
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <iostream>

#include <vector>			 // vector
#include <random>            // mt19937, uniform_int_distribution, random_device

#include <utility>			 // make_pair

#include <boost/thread/thread.hpp>
//#include <boost/lockfree/queue.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>

#include "Chromosome.hpp"
#include "RouletteWheel.hpp"
#include "Result.hpp"

#include "Competitor.hpp"
#include "SafeQueue.hpp"
#include "SafeVector.hpp"

template <class T>
class Manager {
protected:

	unsigned int chromosome_size;

	unsigned int max_generation_number;

	T max_chromosome_value;
	T min_chromosome_value;

	SafeVector<Chromosome<T > > master_population;

	std::vector<Result > master_fitness;

	// Need to have some way of ensure no duplication of solutions
	SafeVector<Chromosome<T> > solutions;

	RouletteWheel rw;

	boost::thread_group fitness_group;

	std::mt19937 rand_engine;
	std::uniform_real_distribution<float> op_dist;
	std::uniform_real_distribution<float> mutation_dist;
	//std::uniform_int_distribution<int> chrom_dist;

	// With this we can limit the number of threads used
	//unsigned int num_threads_used;
	unsigned int max_num_threads;

	boost::atomic<bool> done;
	
	unsigned int num_competitor;
	std::vector<boost::shared_ptr<Competitor<T > > > competitors;

	boost::thread_group competitor_group;
	boost::barrier wall;
	boost::barrier whistle;

	// Fitness function
	double (*fitness_function)(Chromosome<T>);
public:

    // TODO this should take a configuration struct/object that has a lot of default parameters
    // Define the barrier to accept 1 per thread per competitor and 1 per competitor and 1 for the master thread
	/**
	 * Create a non self-adaptive GA manager
	 * @param population_sizes The size of population.
	 * @param chromosome_size The size of each chromosome.
	 * @param max_chromosome_value The maximum possible value for the chromosome.
	 * @param min_chromosome_value The minimum possible value for the chromosome.
	 * @param mutation_rates The mutation rate that is used. This is the likelihood that the
	 * mutation operation is applied. If the self adaptive approach is used then this is
	 * the initial mutation rate.
	 * @param crossover_rates The crossover rate, the likelihood that a chromosome
	 * has the crossover operation applied to it.
	 * @param cloning_rate The cloning rate, the likelihood that a chromosome will be
	 * cloned into the new population.
	 */
	Manager(std::vector<unsigned int > population_sizes, unsigned int chromosome_size, unsigned int max_generation_number,
				T max_chromosome_value, T min_chromosome_value, std::vector<double > mutation_rates,
				std::vector<double > crossover_rates, unsigned int num_competitor, unsigned int num_threads) :
				chromosome_size(chromosome_size), max_generation_number(max_generation_number),
				max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
				num_competitor(num_competitor), max_num_threads(num_threads),
				wall(num_competitor*num_threads + num_competitor + 1), whistle(num_competitor+1) {
				
		//population = std::vector<Chromosome<T > >(population_size);
		initialize(population_sizes, mutation_rates, crossover_rates); 

	}

	~Manager() {

	}

    /**
	 * Run the algorithm for the specified number of generations
	 */	
	unsigned int run(double (*fitness_function)(Chromosome<T>)) {


		for(unsigned int i = 0; i < num_competitor; i++) {
			competitors[i]->initPopulation(this->chromosome_size);

			/*
			std::cout << "Initial Population " << std::endl;
			std::vector<Chromosome<T > > initial_pop;
			competitors[i]->population.getAll(initial_pop);
			for (unsigned int i = 0; i < initial_pop.size(); i++) {
				for (unsigned int j = 0; j < chromosome_size; j++) {
					std::cout << initial_pop[i][j];
					if(j +1 < chromosome_size) {
						std::cout << ",";
					}
				}
				std::cout << '\n';

			}*/
		}
		this->fitness_function = fitness_function;
		wall.wait();
		unsigned int i;
		for(i = 0; i < max_generation_number && !done; i++) {
		//for(unsigned int i = 0; i < 1; i++) {
			//std::cout << "Generation " << i << std::endl;

			// On the last generation
			if(i+1 == max_generation_number) {
			//if(i+1 == 1) {
				referee(true);
			}
			else {
				referee();
			}
			//runGeneration();
			
		}

		done = true;
		//m_cond.notify_all();

		fitness_group.join_all();

		/*
		for(unsigned int i = 0; i < num_competitor; i++) {

			std::cout << "Result Population: " << std::endl;
			std::vector<Chromosome<T > > final_pop;
			competitors[i]->population.getAll(final_pop);
			for (unsigned int i = 0; i < final_pop.size(); i++) {
				for (unsigned int j = 0; j < chromosome_size; j++) {
					std::cout << final_pop[i][j];
					if(j +1 < chromosome_size) {
						std::cout << ",";
					}
				}
				std::cout << '\n';

			}
		}*/
		return i;
	}

	/**
	 * Get another chromosome and apply the fitness function.
	 * @param m The manager the thread is running for.
	 * TODO finish the documentation
	 */
	static void calcFitnessFunction(Manager *m, boost::shared_ptr<Competitor<T > > comp,
		unsigned int start_index, unsigned int problem_size) {

		//std::cout << "Worker Thread range " << start_index << " - " << start_index+problem_size << std::endl;

		unsigned int problem_index;
		std::vector<Result > results;

		m->wall.wait();

		while(!m->done) {

			// Could have a look up table (cache) of recent solutions but this is most likely a completely alternative idea

			//std::cout << "Population Size = " << comp->population.size() << " range " << start_index << " - " << start_index + problem_size-1 << std::endl;
			// Can we trust the user to not change the chromosome? No.
			for(unsigned int i = 0; i < problem_size; i++) {

				//std::cout << "Current = " << start_index + i << std::endl;
				Chromosome<T> t = comp->population.at(start_index+i);

				results.push_back(Result(start_index+i, m->fitness_function(t)));
			}

			// Store them in results
			comp->result_queue.push(results);

			results.clear();

			//std::cout << "Finished working!" << std::endl;

			// Wait for all the threads to complete their fitness functions
			m->wall.wait();

			//std::cout << "Starting breeding" << std::endl;

			// Breed the population
		
			// Each worker thread is responsible for replacing their own sub population of their competitor
			std::vector<Chromosome<T > > sub_population = m->breed(comp->getPopulationSize(),
				comp->getMutationRate(), comp->getCrossoverRate(), problem_size);

			// Join the populations back together.
			comp->population.copy(start_index, sub_population);

			//std::cout << "Finished breeding" << std::endl;
		}
	}

	std::vector<Chromosome<T > > getSolutions() {
		std::vector<Chromosome<T > > temp;
		this->solutions.getAll(temp);
		return temp;
	}


private:

	/**
	 * Setup the necessary variables for genetic algorithm.
	 */
	void initialize(std::vector<unsigned int > population_sizes, std::vector<double > mutation_rates, 
		std::vector<double > crossover_rates) {
		// Create the random objects that will be used
		std::random_device rd;
		rand_engine = std::mt19937(rd());
		op_dist = std::uniform_real_distribution<float>(0.0, 1.0);
		mutation_dist = std::uniform_real_distribution<float>(0.0, 1.0);
		Chromosome<T>::initialize(chromosome_size, min_chromosome_value, max_chromosome_value);
		done = false;

		//std::cout << "Initializing" << std::endl;

		int problem_size;
		int count;

		for(unsigned int j = 0; j < num_competitor; j++) {

			// TODO update this to allow for variations of these three parameters.
			boost::shared_ptr<Competitor<T > > competitor(new Competitor<T >(population_sizes[j],
				mutation_rates[j], crossover_rates[j]));

			//std::cout << "Adding to vector " << std::endl;
			competitors.push_back(competitor);
			problem_size = competitors.back()->getPopulationSize()/max_num_threads;
			count = 0;

			//std::cout << "Competitor " << j << " initializing" << std::endl;

			// Create the number of threads requested. If we are using self adaptive we will require 1 thread for the self adaptive
			for(unsigned int i = 0; i < max_num_threads; i++) {

				if (i+1 == max_num_threads && competitors.back()->getPopulationSize() % max_num_threads != 0) {

					// Handle the case where the problem_size does not evenly divide by the 
					// number of threads available. Last thread receives remainder.
					// This is not the best solution but is passable.
					problem_size = competitors.back()->getPopulationSize() % max_num_threads;
				}

				//std::cout << "Thread number " << i << std::endl;

				//pthread_create(&threadpool.back(), 0, calcFitnessFunction, (void *) this);
				fitness_group.create_thread(boost::bind(calcFitnessFunction, this, competitors.back(), count, problem_size));
				count+= problem_size;
			}

			//std::cout << "competitors " << j << " starting" << std::endl;

			competitor_group.create_thread(boost::bind(runGeneration, this, competitors.back()));
		}

		//std::cout << "Finished initializing" << std::endl;
	}

	/**
	 * Mutate the given chromosome on the likelihood of the mutation rate.
	 * @param chromosome The chromosome to mutate.
	 */
	void mutate(Chromosome<T > &chromosome, double mutation_rate) {
		if(mutation_dist(rand_engine) <= mutation_rate) {
			chromosome.mutate();
		}
	}

	/**
	 * Apply the fitness function to the population. Given the result of the
	 * fitness function, the next population is bred.
	 */
	static void runGeneration(Manager *m, boost::shared_ptr<Competitor<T > > comp) {

		// Get run generation to wait until run() is called
		//m->whistle.wait();

		m->wall.wait();

		while(!m->done) {

			//std::cout << "Tell workers to start" << std::endl;

			comp->fitness_results.clear();

			std::vector<Result > results;
			std::vector<Chromosome<T > > solutions;

			// Main threads will wait for all children to finish executing before proceeding.
			// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value.
			// Wait for all the chromosomes to finish calculating their fitness value.
			while(comp->fitness_results.size() < comp->getPopulationSize()) {
				while(comp->result_queue.popAll(results, false)) {

					comp->fitness_results.push_back(results);

					for(unsigned int i = 0; i < results.size(); i++) {
						// Store the solutions locally 
						//std::cout << "Checking for solutions " << results[i].getResult() << std::endl;
						if(results[i].getResult() == 1.0) {
							//std::cout << "Found Solution" << std::endl;
							solutions.push_back(comp->population.at(results[i].getIndex()));
							//m->done = true;
						}
					}

					// Add the solutions to the master solutions vector
					m->solutions.push_back(solutions);

					results.clear();
					solutions.clear();
				}
			}
			//std::cout << "Workers all finished" << std::endl;

			// TODO consider removing (since worker threads are also going to do the  breeding)
			// Set the workers able to wait once they have finished their work
			//comp->set_waiting(false);

			// Wait for the other competitors to finish mutating
			m->whistle.wait();

			// Wait for the referee to set up the next generation
			m->wall.wait();
		}
				
		//breed(fitness_results); referee
	}

	void referee(bool final=false) {

		//std::cout << "Waiting for fitness eval" << std::endl;

		// Wait for the competitor threads to signal that their populations are ready.
		whistle.wait();

		//std::cout << "Starting set up of breeding" << std::endl;

		// This should prob. be safe a vector and an attribute of Manager.
		master_population.clear();
		master_fitness.clear();
		std::vector<Result > c_fitness;
		std::vector<Chromosome<T > > temp_population;
		unsigned int offset = 0;

		for(unsigned int i = 0; i < competitors.size(); i++) {
			competitors[i]->fitness_results.getAll(c_fitness);
			competitors[i]->population.getAll(temp_population);

			for(unsigned int j = 0; j < competitors[i]->getPopulationSize(); j++) {

				// Add the conversion offset
				c_fitness[j].offsetIndex(offset);

				// Append the value to the master fitness function
				master_fitness.push_back(c_fitness[j]);
			}
			
			// Append the chromosome to the master population
			//master_population.push_back(temp_population);
			master_population.push_back(temp_population);

			// TODO do not clear this, update the population in the worker threads
			//competitors[i]->population.clear();
			
			// The next fitness function master index location is at sum(i=0,i=prev_competitor,fitness_size)
			// e.g. competitor 2's master index location for the local first value (i = 1, c_fitness[0]) is competitor_1.size()
			// and the second local value (i = 1, c_fitness[1]) is competitor_1.size() + 1 and so on.
			offset+= competitors[i]->fitness_results.size();

		}

		rw.init(master_fitness);
		//std::cout << "Setup breeding waiting for next breeding to finish" << std::endl;

		if(final || solutions.size() > 0) {
			done = true;
		}

		// Notify competitors that they are free to proceed.
		wall.wait();
	}

	/**
	 * Prepare the population for the next generation by apply the genetic operations.
	 */
	std::vector<Chromosome<T> > breed(unsigned int population_size, double mutation_rate,
		double crossover_rate, unsigned int problem_size) {

		std::vector<Chromosome<T> > new_population;

		//std::cout << "Breeding" << std::endl;
		//rw.init(fitness);


		// Iterate through the chromosomes
		//while(new_population.size() < population_size) {
		while(new_population.size() < problem_size) {
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			float selected_operation = op_dist(rand_engine);
			unsigned int selected_chromosome = rw.next();

			if(selected_operation <= crossover_rate) {
				// Crossover
				unsigned int other_selected_chromosome = rw.next();

				// If the chromosome selected are the same than there is no point apply the crossover.
				if(other_selected_chromosome != selected_chromosome) {
					master_population.at(selected_chromosome).crossover(master_population.at(other_selected_chromosome), new_population);
				} else {
					master_population.at(selected_chromosome).cloning(new_population);
					master_population.at(other_selected_chromosome).cloning(new_population);
				}

				// Mutate the second chromosome in the crossover
				mutate((*(new_population.end() - 2)), mutation_rate);

				// Handle the case where the new_master_population.size() -1 == master_population size and then crossover is selected.
				if(new_population.size() == problem_size+1) {
					new_population.pop_back();
				}
			}
			else {
				// Clone
                master_population.at(selected_chromosome).cloning(new_population);
			}

			// Mutate the chromosome
			mutate(new_population.back(), mutation_rate);

		}

		//std::cout << "Finished Breeding" << std::endl;

		// Update the population to the new population
		return new_population;
	}
};


#endif /* MANAGER_HPP_ */

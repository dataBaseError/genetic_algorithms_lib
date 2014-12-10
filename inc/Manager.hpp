
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

#include "Chromosome.hpp"
#include "RouletteWheel.hpp"
#include "Result.hpp"

#include "Competitor.hpp"
#include "SafeQueue.hpp"
#include "SafeVector.hpp"

template <class T>
class Manager {
protected:

	unsigned int population_size;
	unsigned int chromosome_size;

	unsigned int max_generation_number;

	T max_chromosome_value;
	T min_chromosome_value;

	double mutation_rate;

	double crossover_rate;

	std::vector<Chromosome<T> > population;

	// Need to have some way of ensure no duplication of solutions
	SafeQueue<Chromosome<T> > solutions;

	RouletteWheel rw;

	//SafeQueue<unsigned int> safe_queue;
	SafeQueue<Result > result_queue;

	boost::thread_group fitness_group;

	std::mt19937 rand_engine;
	std::uniform_real_distribution<float> op_dist;
	std::uniform_real_distribution<float> mutation_dist;
	//std::uniform_int_distribution<int> chrom_dist;

	// With this we can limit the number of threads used
	unsigned int num_threads_used;
	unsigned int max_num_threads;

	// Library flags
	//int number_of_threads = 5;
	boost::atomic<bool> done;
	boost::atomic<bool> waiting;

	boost::condition_variable m_cond;
	boost::mutex mtx_;

	unsigned int num_competitor;
	std::vector<Competitor<T >* > competitors;

	boost::thread_group competitor_group;
	boost::barrier wall;

	// Fitness function
	double (*fitness_function)(Chromosome<T>);
public:

    // TODO this should take a configuration struct/object that has a lot of default parameters
	/**
	 * Create a non self-adaptive GA manager
	 * @param population_size The size of population.
	 * @param chromosome_size The size of each chromosome.
	 * @param max_chromosome_value The maximum possible value for the chromosome.
	 * @param min_chromosome_value The minimum possible value for the chromosome.
	 * @param mutation_rate The mutation rate that is used. This is the likelihood that the
	 * mutation operation is applied. If the self adaptive approach is used then this is
	 * the initial mutation rate.
	 * @param crossover_rate The crossover rate, the likelihood that a chromosome
	 * has the crossover operation applied to it.
	 * @param cloning_rate The cloning rate, the likelihood that a cromosome will be
	 * cloned into the new population.
	 */
	Manager(unsigned int population_size, unsigned int chromosome_size, unsigned int max_generation_number,
				T max_chromosome_value, T min_chromosome_value, double mutation_rate,
				double crossover_rate, unsigned int num_competitor, unsigned int num_threads) : population_size(population_size),
				chromosome_size(chromosome_size), max_generation_number(max_generation_number),
				max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
				mutation_rate(mutation_rate), crossover_rate(crossover_rate),
				population(population_size), num_competitor(num_competitor), 
				max_num_threads(num_threads), wall(num_competitor*num_threads + num_competitor) {
				// Define the barrier to accept 1 per thread per competitor and 1 per competitor
		//population = std::vector<Chromosome<T > >(population_size);
		initialize(); 

	}

	~Manager() {

	}

    /**
	 * Run the algorithm for the specified number of generations
	 */	
	void run(double (*fitness_function)(Chromosome<T>)) {

		/*for(unsigned int i = 0; i < num_competitor; i++) {
			competitors[i]->setFitness(fitness_function);
		}*/
		this->fitness_function = fitness_function;

		initPopulation();

		for(unsigned int i = 0; i < max_generation_number; i++) {
			//std::cout << "Generation " << i << std::endl;
			//runGeneration();
		}

		done = true;
		m_cond.notify_all();

		fitness_group.join_all();
		
	}

	/**
	 * Get another chromosome and apply the fitness function.
	 * @param m The manager the thread is running for.
	 * TODO finish the documentation
	 */
	static void calcFitnessFunction(Manager *m, Competitor<T > *comp, unsigned int start_index, unsigned int problem_size) {

		unsigned int problem_index;

		std::vector<Result > results;
		while(!comp->done) {

			if(comp->wait_for_work()) {
				break;
			}

			// Can we trust the user to not change the chromosome? No.
			for(unsigned int i = 0; i < problem_size; i++) {

				Chromosome<T> t = comp->population[start_index+i];

				results.push_back(Result(start_index+i, m->fitness_function(t)));
			}

			// Store them in results
			comp->result_queue.push(results);

			results.clear();

			// Wait for all the threads to complete their fitness functions
			m->wall.wait();

			// Breed the population

			// Each worker thread is responsible for replacing their own sub population of their competitor
		}
	}

	std::vector<Chromosome<T > > getSolutions() {
		return this->solutions;
	}

    // TODO make this private (after testing)
	/**
	 * Create an initial population of random chromosomes.
	 */
	void initPopulation() {
		Chromosome<unsigned int>::initPopulation(this->population, this->population_size, this->chromosome_size);
	}

    /**
	 * Returns the population size specified for the manager class.
     * @return The population size
     */
	unsigned int size() {
		return this->population_size;
	}

    /**
     * Returns the entire chromosome population.
     * @return The chromosome population
     */
	std::vector<Chromosome<T> > getPopulation() {
		return this->population;
	}

private:

	bool wait_for_work() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		if(!done) {
			if(!waiting) {
				m_cond.wait(lock);
			}			
		}
		else {
			return true;
		}
		return false;
	}

	void set_waiting(bool val){
		boost::unique_lock<boost::mutex> guard(mtx_);
		waiting = val;
	}

	/**
	 * Setup the necessary variables for genetic algorithm.
	 */
	void initialize() {
		// Create the random objects that will be used
		std::random_device rd;
		rand_engine = std::mt19937(rd());
		op_dist = std::uniform_real_distribution<float>(0.0, 1.0);
		mutation_dist = std::uniform_real_distribution<float>(0.0, 1.0);
		Chromosome<T>::initialize(chromosome_size, min_chromosome_value, max_chromosome_value);
		done = false;

		int problem_size;
		int count;

		for(unsigned int j = 0; j < num_competitor; j++) {

			// TODO update this to allow for variations of these three parameters.
			Competitor<T > competitor(population_size, mutation_rate, crossover_rate);

			problem_size = competitor.getPopulationSize()/max_num_threads;
			count = 0;

			// Create the number of threads requested. If we are using self adaptive we will require 1 thread for the self adaptive
			for(unsigned int i = 0; i < max_num_threads; i++) {

				if (i+1 == max_num_threads && competitor.getPopulationSize() % max_num_threads != 0) {

					// Handle the case where the problem_size does not evenly divide by the 
					// number of threads available. Last thread receives remainder.
					// This is not the best solution but is passable.
					problem_size = competitor.getPopulationSize() % max_num_threads;
				}

				//pthread_create(&threadpool.back(), 0, calcFitnessFunction, (void *) this);
				fitness_group.create_thread(boost::bind(calcFitnessFunction, this, &competitor, count, problem_size));
				count+= problem_size;
			}

			competitor_group.create_thread(boost::bind(runGeneration, this, &competitor));
			competitors.push_back(&competitor);
		}
	}

	/**
	 * Mutate the given chromosome on the likelihood of the mutation rate.
	 * @param chromosome The chromosome to mutate.
	 */
	void mutate(Chromosome<T > &chromosome) {
		if(mutation_dist(rand_engine) <= mutation_rate) {
			chromosome.mutate();
		}
	}

	/**
	 * Apply the fitness function to the population. Given the result of the
	 * fitness function, the next population is bred.
	 */
	static void runGeneration(Manager *m, Competitor<T > *comp) {

		comp->set_waiting(true);
		comp->start_working();

		comp->fitness_results.clear();
		
		std::vector<Result > results;
		std::vector<Result > solutions;

		// Main threads will wait for all children to finish executing before proceeding.
		// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value.
		// Wait for all the chromosomes to finish calculating their fitness value.
		while(comp->fitness_results.size() < comp->getPopulationSize()) {
			while(comp->result_queue.popAll(results, false)) {

				comp->fitness_results.push_back(results[i]);

				for(unsigned int i = 0; i < results.size(); i++) {
					// Store the solutions locally
					if(results[i].getResult() == 1.0) {
						solutions.push_back(comp->population[results[i].getIndex()]);
					}
				}

				// Add the solutions to the master solutions vector
				m->solutions.push_back(solutions);
				
				results.clear();
				solutions.clear();
			}
		}

		// TODO consider removing (since worker threads are also going to do the breeding)
		// Set the workers able to wait once they have finished their work
		comp->set_waiting(false);

		// Need to create the master fitness roulette and the master population

		m->wall.wait();
				
		//breed(fitness_results); referee
	}

	void referee() {

		// Wait for the competitor threads to signal that their populations are ready.

		std::vector<Result > master_fitness;
		std::vector<Chromosome<T > > master_population;

		std::vector<Result > c_fitness;
		unsigned int offset = 0;

		for(unsigned int i = 0; i < competitors.size(); i++) {
			competitors[i]->fitness_results.getAll(c_fitness);

			for(unsigned int j = 0; j < c_fitness.size(); j++) {
				c_fitness[j].offsetIndex(offset)
			}
			
			// The next fitness function master index location is at sum(i=0,i=prev_competitor,fitness_size)
			// e.g. competitor 2's master index location for the local first value (c_fitness[0]) is competitor_1.size()
			// and the second local value (c_fitness[1]) is competitor_1.size()+1 and so on.
			offset+= c_fitness.size();

			// Append the c_fitness to the master_fitness
			master_fitness.insert(master_fitness.end(), c_fitness.begin(), c_fitness.end());
		}
	}

	/**
	 * Prepare the population for the next generation by apply the genetic operations.
	 */
	void breed(std::vector<Result > &fitness) {
		std::vector<Chromosome<T> > new_population;

		//std::cout << "Breeding" << std::endl;
		rw.init(fitness);

		// Iterate through the chromosomes
		while(new_population.size() < population_size) {
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			float selected_operation = op_dist(rand_engine);
			unsigned int selected_chromosome = rw.next();

			
			if(selected_operation <= crossover_rate) {
				// Crossover
				unsigned int other_selected_chromosome = rw.next();

				// If the chromosome selected are the same than there is no point apply the crossover.
				if(other_selected_chromosome != selected_chromosome) {
					population[selected_chromosome].crossover(population[other_selected_chromosome], new_population);
				} else {
					population[selected_chromosome].cloning(new_population);
					population[other_selected_chromosome].cloning(new_population);
				}

				// Mutate the second chromosome in the crossover
				mutate((*(new_population.end() - 2)));

				// Handle the case where the new_population.size() -1 == popluation size and then crossover is selected.
				if(new_population.size() == population_size+1) {
					new_population.pop_back();
				}
			}
			else {
				// Clone
                population[selected_chromosome].cloning(new_population);
			}

			// Mutate the chromosome
			mutate(new_population.back());

		}

		//std::cout << "Finished Breeding" << std::endl;

		// Update the population to the new population
		population = new_population;
	}
};


#endif /* MANAGER_HPP_ */

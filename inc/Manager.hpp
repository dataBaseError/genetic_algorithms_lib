
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <iostream>

#include <vector>			 // vector
#include <random>            // mt19937, uniform_int_distribution, random_device

#include <utility>			 // make_pair

#include <boost/thread/thread.hpp>
//#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>

#include "Chromosome.hpp"
#include "RouletteWheel.hpp"
#include "Result.hpp"

#include "SafeQueue.hpp"

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
	std::vector<Chromosome<T> > solutions;

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
				double crossover_rate, unsigned int num_threads) : population_size(population_size),
				chromosome_size(chromosome_size), max_generation_number(max_generation_number),
				max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
				mutation_rate(mutation_rate), crossover_rate(crossover_rate),
				population(population_size), max_num_threads(num_threads){
		population = std::vector<Chromosome<T > >(population_size);
		initialize(); 

	}

	~Manager() {

	}

    /**
	 * Run the algorithm for the specificed number of generations
	 */	
	void run(double (*fitness_function)(Chromosome<T>)) {
		this->fitness_function = fitness_function;

		initPopulation();

		for(unsigned int i = 0; i < max_generation_number; i++) {
			//std::cout << "Generation " << i << std::endl;
			runGeneration();
		}

		done = true;
		m_cond.notify_all();

		fitness_group.join_all();
		
	}

	/**
	 * Get another chromosome and apply the fitness function.
	 * @param param The manager the thread is running for.
	 */
	static void calcFitnessFunction(Manager *m, unsigned int start_index, unsigned int problem_size) {

		unsigned int problem_index;

		std::vector<Result > results;
		while(!m->done) {

			if(m->wait_for_work()) {
				break;
			}

			// Can we trust the user to not change the chromosome? No.
			for(unsigned int i = 0; i < problem_size; i++) {

				Chromosome<T> t = m->population[start_index+i];

				results.push_back(Result(start_index+i, m->fitness_function(t)));
			}

			// Store them in results
			m->result_queue.push(results);

			results.clear();

			//TODO consider a barrier to make all the threads wait until all other threads are complete and then apply breeding in the threads.
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

		int problem_size = population_size/max_num_threads;
		int count = 0;

		// Create the number of threads requested. If we are using self adaptive we will require 1 thread for the self adaptive
		for(unsigned int i = 0; i < max_num_threads; i++) {

			if (i+1 == max_num_threads && population_size % max_num_threads != 0) {

				// Handle the case where the problem_size does not evenly divide by the 
				// number of threads available. Last thread receives remainder.
				// This is not the best solution but is passible.
				problem_size = population_size % max_num_threads;
			}

			//pthread_create(&threadpool.back(), 0, calcFitnessFunction, (void *) this);
			fitness_group.create_thread(boost::bind(calcFitnessFunction, this, count, problem_size));
			count+= problem_size;
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
	void runGeneration() {

		set_waiting(true);
		m_cond.notify_all();

		// Main thread will wait for all children to finish executing before proceeding.
		// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value.
		std::vector<Result >fitness_results;

		// Wait for all the chromosomes to finish calculating their fitness value.
		std::vector<Result > results;
		while(fitness_results.size() < population_size) {
			while(result_queue.popAll(results, false)) {

				for(unsigned int i = 0; i < results.size(); i++) {
					fitness_results.push_back(results[i]);

					//TODO add results to solutions.
					if(results[i].getResult() == 1.0) {
						solutions.push_back(population[results[i].getIndex()]);
					}
				}
				results.clear();
				
			}
		}

		set_waiting(false);
				
		breed(fitness_results);
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

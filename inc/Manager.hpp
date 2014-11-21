
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <iostream>

#include <vector>			 // vector
#include <random>            // mt19937, uniform_int_distribution, random_device

#include <utility>			 // make_pair

#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>

#include "Chromosome.hpp"
#include "RouletteWheel.hpp"
#include "Result.hpp"

template <class T>
class Manager {
protected:

	unsigned int population_size;
	unsigned int chromosome_size;

	unsigned int max_generation_number;

	T max_chromosome_value;
	T min_chromosome_value;

	bool use_self_adaptive;
	double mutation_rate;
	double mutation_change_rate;
	double similarity_index;

	double crossover_rate;

	std::vector<Chromosome<T> > population;

	// Need to have some way of ensure no duplication of solutions
	std::vector<Chromosome<T> > solutions;

	RouletteWheel rw;

	boost::lockfree::queue<Result > result_queue;
	boost::lockfree::queue<unsigned int> safe_queue;
	//SafeQueue<unsigned int> safe_queue;
	//SafeQueue<std::pair<unsigned int, std::pair<double, bool> > > result_queue;

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

	// Fitness function
	double (*fitness_function)(Chromosome<T>);

public:


    // TODO this should take a configuration struct/object that has a lot of default parameters
	/**
	 * Create a manager for GA.
	 * @param population_size The size of population.
	 * @param chromosome_size The size of each chromosome.
	 * @param max_chromosome_value The maximum possible value for the chromosome.
	 * @param min_chromosome_value The minimum possible value for the chromosome.
	 * @param use_self_adaptive Whether to use a self adaptive approach. If this is true
	 * then the mutation rate will be updated based on the similarity of the population.
	 * @param mutation_rate The mutation rate that is used. This is the likelihood that the
	 * mutation operation is applied. If the self adaptive approach is used then this is
	 * the initial mutation rate.
	 * @param mutation_change_rate The rate by which the mutation rate will be updated.
	 * If use_self_adaptive is set to false then this parameter is ignored.
	 * @param similarity_index The threshold at which the similarity mutation rate is
	 * updated.
	 * @param crossover_rate The crossover rate, the likelihood that a chromosome
	 * has the crossover operation applied to it.
	 * @param cloning_rate The cloning rate, the likelihood that a cromosome will be
	 * cloned into the new population.
	 */
	Manager(unsigned int population_size, unsigned int chromosome_size, unsigned int max_generation_number,
			T max_chromosome_value, T min_chromosome_value, bool use_self_adaptive,
			double mutation_rate, double mutation_change_rate, double similarity_index,
			double crossover_rate, unsigned int num_threads) : population_size(population_size),
			chromosome_size(chromosome_size), max_generation_number(max_generation_number),
			max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
			use_self_adaptive(use_self_adaptive), mutation_rate(mutation_rate),
			mutation_change_rate(mutation_change_rate), similarity_index(similarity_index),
			crossover_rate(crossover_rate), max_num_threads(num_threads), safe_queue(population_size), 
			result_queue(population_size) {
		population = std::vector<Chromosome<T > >(population_size);
		initialize();
	}

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
				use_self_adaptive(false), mutation_rate(mutation_rate), mutation_change_rate(0), 
				similarity_index(0), crossover_rate(crossover_rate), population(population_size),
				max_num_threads(num_threads){

		initialize(); 

	}

	~Manager() {

        // TODO if object no longer exists what does finished do?
		//finished = true;
		//safe_queue.finish(threadpool.size());
	}

    /**
	 * Run the algorithm for the specificed number of generations
	 */	
	void run(double (*fitness_function)(Chromosome<T>)) {
		this->fitness_function = fitness_function;

		initPopulation();

		for(unsigned int i = 0; i < max_generation_number; i++) {
			std::cout << "Starting Generation " << i << std::endl;
			runGeneration();
		}
		done = true;
	}

    // TODO make this private (after testing)
	/**
	 * Prepare the population for the next generation by apply the genetic operations.
	 */
	void breed(std::vector<Result > &fitness) {
		std::vector<Chromosome<T> > new_population;

		std::cout << "Breeding" << std::endl;

		std::vector<std::pair<unsigned int, double > > list;
		for(unsigned int i = 0; i < fitness.size(); i ++) {
			list.push_back(std::pair<unsigned int, double >(fitness[i].index, fitness[i].result));
		}
		rw.init(list);

		// Iterate through the chromosomes
		while(new_population.size() < population_size) {	
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			float selected_operation = op_dist(rand_engine);
			unsigned int selected_chromosome = rw.next();
			if(selected_operation <= crossover_rate) {
				// Crossover
				unsigned int other_selected_chromosome = rw.next();

				// TODO add a attribute to allow the user to decided whether to have redundent crossover
				// Since a crossover operation between the same chromosome is the same as cloning we should pick another one
				while(other_selected_chromosome == selected_chromosome) {
					other_selected_chromosome = rw.next();
				}

				population[selected_chromosome].crossover(population[other_selected_chromosome], new_population);

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

		// Update the population to the new population
		population = new_population; // Check if this creates a shallow copy (since we aren't going to change new_population so we just want it to be deallocated at the end of the method).

	}

	/**
 	 * Apply the self-adaptive operation to determine the similarity of the chromosomes
	 * and update the mutation rate accordingly.
	 *
	 * This might be best fit to have 1 thread designated for it (and if more are available use more).
	 * Since the fitness function is most likely the longest process, if we complete the fitness functions
	 * concurrently and then to the selfadaptive (which is a sort and then a comparison) nlogn + n. So by
	 * doing as many fitness function calls (available_threads - 2, 1 for self adaptive and 1 for fitness
	 * function manager) we can then call self adaptive as well and and therefore if the fitness functions
	 * take a long time to execute (or pop_size > available_threads - 2) then we will execute some fitness
	 * function but will actual finish the self adaptive. This means we wont be doing do all fitness functions
	 * then self adaptive (aka approx. fitness function execution time * pop_size/(available_threads - 1) + (self adaptive execution time)) rather we'll
	 * do (some) fitness functions and self adaptive and then remaining as threads become available (approx. fitness function execution * pop_size - 1 / (available_threads - 2) + .
	 */
	void selfAdapt() {		
		// Determine how similar the chromosome is to the rest of the population

		// Sequencial solution
		// Iterate through the chromosomes
			// Calculate the current chromosome with each of the others in the population

		// Based on the similarity of the population identify adjust the mutaiton rate
			//if the similarity is too high (above threshold), increase mutation rate
			//if similarity is too low (below threshold), decrease mutation rate
			//otherwise (same as threshold), do nothing
	}

	/**
	 * Apply the self adaptive function.
	 * @param param The manager class.
	 */
	/*static void *applySelfAdaptive(void *param) {
		if(param != NULL) {
			Manager * m = (Manager *) param;
			while(true) {
				pthread_mutex_lock(&m->self_adapt_lock);
				pthread_cond_wait(&m->self_adapt_wait, &m->self_adapt_lock);
				pthread_mutex_unlock(&m->self_adapt_lock);

				if(m->finished) {
					break;
				}

				m->selfAdapt();

				// proceed with calculating some fitness functions
			}
			
		}
		return NULL;
	}*/

	/**
	 * Get another chromosome and apply the fitness function.
	 * @param param The manager the thread is running for.
	 */
	static void *calcFitnessFunction(void *param) {
		if(param != NULL) {
			Manager * m = (Manager *) param;
			unsigned int problem_index;
			while(!m->done) {

				while(!m->safe_queue.empty() && m->safe_queue.pop(problem_index) && problem_index < m->population_size) {
					//std::cout << "Grabbing a Chromosome " << std::endl;
					// Can we trust the user to not change the chromosome? No.
					Chromosome<T> t = m->population[problem_index];

					//std::pair<unsigned int, double >temp (problem_index, m->fitness_function(t));

					m->result_queue.push(Result(problem_index, m->fitness_function(t)));
				}
				//std::cout << "Nothing to grab" << std::endl;
			}
		}

		return NULL;
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

		// Create the number of threads requested. If we are using self adaptive we will require 1 thread for the self adaptive
		for(unsigned int i = 0; i < max_num_threads - use_self_adaptive; i++) {

			//pthread_create(&threadpool.back(), 0, calcFitnessFunction, (void *) this);
			fitness_group.create_thread(boost::bind(calcFitnessFunction, (void *) this));

			// TODO decide if this is useful
			//this->num_threads_used++;
		}

		/*
		if(use_self_adaptive) {
			pthread_mutex_init(&self_adapt_lock, 0);
			pthread_cond_init(&self_adapt_wait, 0);

			pthread_t self_adapt;
			pthread_create(&self_adapt, 0, applySelfAdaptive, (void *)this);
		}*/
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
	 *
	 * There are several approaches we could take to mapping the number of available threads to the problems:
	 * Each thread is given a problem to solve and will pull off another problem once it is finished.
	 * 	- Adv. Can effectively balance the load
	 * 	- Adv. Easy to implement (with shared memory which threads have).
	 * 	- Dis. Requires more communication
	 * 	- Dis. Requires a manager thread
	 */
	void runGeneration() {

		// Given that threads are going to be used:
			// - Create a thread pool and attempt to assign a reduced set of chromosomes to each thread
			// - Since each chromosome is independent for the execution of the fitness function they can be 1 thread per chromosome (hypothedically)
		for(unsigned int i = 0; i < this->population_size; i++) {
			// Push on the index for each chromosome in the population.
			std::cout << "Pushing population value " << i << std::endl;
			safe_queue.push(i);
		}
		// Could potentially have a second generic method that you could use to apply heuristics to a found solution.
			// eg; in N-Queens you can rotate the board in order to find more solutions.

		// Can be done concurrently with the fitness function
		/*if(use_self_adaptive) {
				// Create a thread to handle selfAdapt()
				pthread_mutex_lock(&self_adapt_lock);
				pthread_cond_signal(&self_adapt_wait);
				pthread_mutex_unlock(&self_adapt_lock);
		}*/

		//fitness_group.join_all();

		// Main thread will wait for all children to finish executing before proceeding.
		// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value.
		std::vector<Result >fitness_results;

		// Wait for all the chromosomes to finish calculating their fitness value.
		Result rel;
		while(fitness_results.size() < population_size) {
			while(result_queue.pop(rel)) {
				fitness_results.push_back(rel);
				//TODO add results to solutions.
			}
		}

		breed(fitness_results);
	}
};


#endif /* MANAGER_HPP_ */

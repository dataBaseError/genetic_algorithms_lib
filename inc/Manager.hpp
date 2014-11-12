
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <vector>			 // vector
#include <random>            // mt19937, uniform_int_distribution, random_device
#include <pthread.h>		 // pthreads
#include <utility>			 // make_pair

#include "Chromosome.hpp"
#include "RouletteWheel.hpp"
#include "SafeQueue.hpp"

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

	// TODO check if clonning rate is by convention (1 - (mutation_rate + crossover)).
	//double clonning_rate;

	double crossover_rate;

	std::vector<Chromosome<T > > population;

	// Need to have some way of ensure no duplication of solutions
	std::vector<Chromosome<T > > solutions;

	RouletteWheel rw;
	RouletteWheel op_roulettewheel;

	SafeQueue<unsigned int > safe_queue;
	SafeQueue<std::pair<unsigned int, double > > result_queue;

	std::mt19937 rand_engine;
	std::uniform_real_distribution<float> op_dist;
	std::uniform_real_distribution<float> mutation_dist;
	//std::uniform_int_distribution<int> chrom_dist;

	// With this we can limit the number of threads used
	unsigned int max_num_threads;
	unsigned int num_threads_used;

	// Library flags
	int number_of_nodes = 0;
	int number_of_threads = 5;

	double (*fitness_function)(Chromosome<T >);

public:
	pthread_mutex_t self_adapt_lock;
	pthread_cond_t self_adapt_wait;


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
	 * @param clonning_rate The clonning rate, the likelihood that a cromosome will be
	 * cloned into the new population.
	 */
	Manager(unsigned int population_size, unsigned int chromosome_size, unsigned int max_generations_number,
			T max_chromosome_value, T min_chromosome_value, bool use_self_adaptive,
			double mutation_rate, double mutation_change_rate, double similarity_index,
			double crossover_rate) : population_size(population_size),
			chromosome_size(chromosome_size), max_generation_number(max_generation_number),
			max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
			use_self_adaptive(use_self_adaptive), mutation_rate(mutation_rate),
			mutation_change_rate(mutation_change_rate), similarity_index(similarity_index),
			crossover_rate(crossover_rate) {
		population = std::vector<Chromosome<T > >(population_size);
		initialize();
	}

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
	 * @param clonning_rate The clonning rate, the likelihood that a cromosome will be
	 * cloned into the new population.
	 */
	Manager(unsigned int population_size, unsigned int chromosome_size, unsigned int max_generation_number,
				T max_chromosome_value, T min_chromosome_value, double mutation_rate,
				double crossover_rate) : population_size(population_size),
				chromosome_size(chromosome_size), max_generation_number(max_generation_number),
				max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
				use_self_adaptive(false), mutation_rate(mutation_rate), mutation_change_rate(0), 
				similarity_index(0), crossover_rate(crossover_rate), population(population_size) {

		initialize(); 

	}

	~Manager() {
		pthread_mutex_destroy(&self_adapt_lock);
		pthread_cond_destroy(&self_adapt_wait);
	}

        /**
	 * Run the algorithm for the specificed number of generations
	 */	
	void run(double (*fitness_function)(Chromosome<T >)) {
		this->fitness_function = fitness_function;
		for(unsigned int i = 0; i < max_generation_number; i++) {
			runGeneration();
		}
	}

	/**
	 * Apply the fitness function to the population. Given the result of the
	 * fitness function, the next population is bread.
	 *
	 * There are several approaches we could take to mapping the number of available threads to the problems:
	 * 1. Use static division of work. That is to say, divide the work up as evenly as possible ahead of problem solving.
	 * eg. 1 thread is given pop_size / (available_threads - 1) chromosomes to solve the fitness function for.
	 * 	- Adv. less communication and no need for a management thread.
	 * 	- Adv. Easy to implement.
	 * 	- Dis. if more threads become available become available (most likely case, they finish their set of problems early)
	 * 	  then they will not be taken advantage of.
	 * 2. Use dynamic division of work. Assign the threads a work as the become available. This can happen in two ways;
	 * 2.1 Use static division and then load balance if new threads become available for work (while more work is pending).
	 * 	- Adv. basically the same as the static except take advantage of available resources
	 * 	- Dis. Re-balancing will take more communication.
	 * 	- Dis. Harder to implement (have to create a balancer which will transfer queued problems to the free thread.
	 * 	- Dis. Requires a managment thread.
	 * 2.2 Each thread is given a problem to solve and will pull off another problem once it is finished.
	 * 	- Adv. Can effectively balance the load
	 * 	- Adv. Easy to implement (with shared memory which threads have).
	 * 	- Dis. Requires more communication
	 * 	- Dis. Requires a manager thread
	 */
	void runGeneration() {

		// Fitness function would be a generic method passed as an argument that is apply to each chromosome
		//fptr(population);

		for(unsigned int i = 0; i < this->population_size; i++) {
			// Push on the index for each chromosome in the population.
			safe_queue.push(i);
		}
		
		// Given that threads are going to be used:
			// - Create a thread pool and attempt to assign a reduced set of chromosomes to each thread
			// - Since each chromosome is independent for the execution of the fitness function they can be 1 thread per chromosome (hypothedically)

		// Could potentially have a second generic method that you could use to apply heuristics to a found solution.
			// eg; in N-Queens you can rotate the board in order to find more solutions.

		// Can be done concurrently with the fitness function
		if(use_self_adaptive) {
				// Create a thread to handle selfAdapt()
				pthread_mutex_lock(&self_adapt_lock);
				pthread_cond_signal(&self_adapt_wait);
				pthread_mutex_unlock(&self_adapt_lock);
		}

		// Main thread will wait for all children to finish executing before proceeding.
		// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value.
		std::map<unsigned int, double > fitness_results;
		std::pair<unsigned int, double > result;

		// Wait for all the chromosomes to finish calculating their fitness value.
		while(fitness_results.size() < population_size) {
			result = result_queue.waitToPop();
			fitness_results[result.first] = result.second;
		}

		breed(fitness_results); 
	}

	/**
	 * Prepare the population for the next generation by apply the genetic operations.
	 */
	void breed(std::vector<std::pair<unsigned int, double> > &fitness) {
		std::vector<Chromosome<T > > new_population;
		rw.init(fitness);

		// Iterate through the chromosomes
		while(new_population.size() < population_size) {	
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			float selected_operation = op_dist(rand_engine);
			unsigned int selected_chromosome = rw.next();
			if(selected_operation <= crossover_rate) {
				// Crossover
				unsigned int other_selected_chromosome = rw.next();

				// TODO add a attribute to allow the user to decided whether to have redundent crossover
				// Since a crossover operation between the same chromosome is the same as clonning we should pick another one
				while(other_selected_chromosome == selected_chromosome) {
					other_selected_chromosome = rw.next();
				}

				population[selected_chromosome].crossover(population[other_selected_chromosome], new_population);

				// Mutate the second chromosome in the crossover
				mutate((*(new_population.end() - 2)));
			}
			else {
				// Clone
				population[selected_chromosome].clone(new_population);
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

	// Instead of taking a fitness function it might be better to take a function pointer and call that function for the fitness function
	// void *fitness_function(std::vector<Chromosome<T > > population);

	/*static void *fitness_function(std::vector<Chromosome<T > > population) {

		return NULL;
	}*/

	static void *applySelfAdaptive(void *param) {
		if(param != NULL) {
			Manager * m = (Manager *) param;
			while(true) {
				pthread_mutex_lock(&m->self_adapt_lock);
				pthread_cond_wait(&m->self_adapt_wait, &m->self_adapt_lock);
				pthread_mutex_unlock(&m->self_adapt_lock);

				m->selfAdapt();
			}
			
		}
		// TODO Deallocate m
		return NULL;
	}

	static void *calcFitnessFunction(void *param) {
		if(param != NULL) {
			Manager * m = (Manager *) param;
			while(true) {
				unsigned int problem_index = m->safe_queue.waitToPop();

				// Can we trust the user to not change the chromosome? No.
				Chromosome<T > t = m->population[problem_index];
				double result = m->fitness_function(t);
				m->result_queue.push(std::make_pair(problem_index, result));
			}

		}
		// TODO Deallocate m

		return NULL;
	}

private:
	void initialize() {
		// Create the random objects that will be used
		std::random_device rd;
		rand_engine = std::mt19937 (rd());
		op_dist = std::uniform_real_distribution<float> (0.0, 1.0);
		mutation_dist = std::uniform_real_distribution<float> (0.0, 1.0);
		//chrom_dist = std::uniform_int_distribution<int> (MINIMUM_NUMBER, population_size-1);
		Chromosome<T >::initialize(chromosome_size, min_chromosome_value, max_chromosome_value);

		// TODO create simple thread pool (vector of threads) which will all be given calcFitnessFunction as their function

		if(use_self_adaptive) {
			pthread_mutex_init(&self_adapt_lock, 0);
			pthread_cond_init(&self_adapt_wait, 0);

			pthread_t self_adapt;
			pthread_create(&self_adapt, 0, applySelfAdaptive, (void *)this);
		}
	}

	void mutate(Chromosome<T > &chromosome) {
		if(mutation_dist(rand_engine) <= mutation_rate) {
			chromosome.mutate();
		}
	}
};


#endif /* MANAGER_HPP_ */


#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <vector>
#include <map>
#include <random>
#include <pthread.h>

#include "Chromosome.hpp"

#define MINIMUM_NUMBER 0

bool use_mpi = false;
int number_of_nodes = 0;
int number_of_threads = 5;

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

	double clonning_rate;

	double crossover_rate;

	std::vector<Chromosome<T > > population;

	// Need to have some way of ensure no duplication of solutions
	std::vector<Chromosome<T > > solutions;

	std::mt19937 rand_engine;
	std::uniform_real_distribution<> op_dist;
	std::uniform_int_distribution<int> chrom_dist;

	// With this we can limit the number of threads used
	unsigned int max_num_threads;
	unsigned int num_threads_used;

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
			double crossover_rate, double clonning_rate) : population_size(population_size),
			chromosome_size(chromosome_size), max_generation_number(max_generation_number),
			max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
			use_self_adaptive(use_self_adaptive), mutation_rate(mutation_rate),
			mutation_change_rate(mutation_change_rate), similarity_index(similarity_index),
			clonning_rate(clonning_rate), crossover_rate(crossover_rate),
			population(population_size) {

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
				double crossover_rate, double clonning_rate) : population_size(population_size),
				chromosome_size(chromosome_size), max_generation_number(max_generation_number),
				max_chromosome_value(max_chromosome_value), min_chromosome_value(min_chromosome_value),
				use_self_adaptive(false), mutation_rate(mutation_rate), mutation_change_rate(0), 
				similarity_index(0), clonning_rate(clonning_rate), crossover_rate(crossover_rate),
				population(population_size) {

		initialize(); 

	}

        /**
	 * Run the algorithm for the specificed number of generations
	 */	
	void run(void (* fptr)(void*)) {
		for(unsigned int i = 0; i < max_generation_number; i++) {
			runGeneration(&fptr);
		}
	}

	/**
	 * Apply the fitness function to the population. Given the result of the
	 * fitness function, the next population is bread.
	 */
	void runGeneration(void (* fptr)(void*)) {

		// Fitness function would be a generic method passed as an argument that is apply to each chromosome
		fptr(population);
		
		// Given that threads are going to be used:
			// - Create a thread pool and attempt to assign a reduced set of chromosomes to each thread
			// - Since each chromosome is independent for the execution of the fitness function they can be 1 thread per chromosome (hypothedically)
		// Given that MPI is being used
			// - Reduce the chromosomes into smaller sets and assign them to a node
			// - Each node can then divide the subpopulation further (possibly to a per-chromosome per thread) depending on availablity. (this step is similar to the second step with threading.
			// Apply the fitness function

		// Could potentially have a second generic method that you could use to apply heuristics to a found solution.
			// eg; in N-Queens you can rotate the board in order to find more solutions.

		// Can be done concurrently with the fitness function
		
		if(use_self_adaptive) {		
			if(use_mpi) {
				// Use mpi to apply selfAdapt()
			}
			else {
				// Create a thread to handle selfAdapt()
				pthread_mutex_lock(&self_adapt_lock);
				pthread_cond_signal(&self_adapt_wait);
				pthread_mutex_unlock(&self_adapt_lock);
			}
		}


		// Main thread will wait for all children to finish executing before proceeding.
		// Construct the list of results for the fitness function in a map which maps the chromosome's index to the chromosome's fitness value. The type of the result of the fitness function can be any desired type however primitive is desired. eg:
		std::map<unsigned int, int > fitness_results;	
		breed(fitness_results); 
	}

	/**
	 * Prepare the population for the next generation by apply the genetic operations.
	 */
	template <typename F>
	void breed(std::map<unsigned int, F > fitness_result) {
		std::vector<Chromosome<T > > new_population;

		// Iterate through the chromosomes
		while(new_population.size() < population_size) {	
			// TODO Using a specific selection (eg roulette wheel without replacement) to pick a chromosome
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			int selected_operation = 0;
			int selected_chromosome = getRandChromosome(); 
			if(selected_operation == 0) {
				// Mutation
				population[selected_chromosome].mutation(new_population);
			}
			else if(selected_operation == 1) {
				// Crossover 
				// Pick another chromosome from the population that is not the already selected chromosome. (TODO can the chromosome be the same as the initally selected on?)
				int other_selected_chromosome = getRandChromosome(selected_chromosome);

				population[selected_chromosome].crossover(population[other_selected_chromosome], new_population);

			}
			else {
				// Clone
				population[selected_chromosome].clone(new_population);
			}
		}

		// Update the population to the new population
		population = new_population; // Check if this creates a shallow copy (since we aren't going to change new_population so we just want it to be deallocated at the end of the method).

	}

	/**
 	 * Apply the self-adaptive operation to determine the similarity of the chromosomes
	 * and update the mutation rate accordingly.
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
		// Deallocate m
	}

private:
	void initialize() {
		// Create the random objects that will be used
		std::random_device rd;
		rand_engine = std::mt19937 (rd());
		op_dist = std::uniform_real_distribution<> (0, 100);
		chrom_dist = std::uniform_int_distribution<int> (MINIMUM_NUMBER, population_size-1);
		Chromosome<T >::initialize(chromosome_size, min_chromosome_value, max_chromosome_value);

		if(!use_mpi && use_self_adaptive) {
			pthread_mutex_init(&self_adapt_lock, 0);
			pthread_cond_init(&self_adapt_wait, 0);

			pthread_t self_adapt;
			pthread_create(&self_adapt, 0, applySelfAdaptive, (void *)this);
		}
	}

	/**
         * Get a number between 0 and population.size() - 1
	 * @return A number between 0 and population.size()-1
         */
	unsigned int getRandChromosome() {
		return chrom_dist(rand_engine);
	}

	/**
	 * Get a random number between 0 and population.size()-1 that is not index
	 * @param index The only value within the range that the return cannot be.
	 * @return A number within the defined range that is not index.
	 */
	unsigned int getRandChromosome(unsigned int index) {
		unsigned int val = getRandChromosome();

		while(val == index) {
			val = getRandChromosome();	
		}
		return val;
	}
};


#endif /* MANAGER_HPP_ */

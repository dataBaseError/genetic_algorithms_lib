
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <vector>
#include <random>

//#include "FitnessFunction.hpp"
#include "Chromosome.hpp"

template <class T>
class Manager {
protected:

	unsigned int population_size;
	unsigned int chromosome_size;

	T max_chromosome_value;
	T min_chromosome_value;

	bool use_self_adaptive;
	double mutation_rate;
	double mutation_change_rate;
	double similarity_index;

	double clonning_rate;

	double crossover_rate;

	std::vector<Chromosome<T > > population;

	std::mt19937 rand_engine;
	std::uniform_real_distribution<> op_dist;
	std::uniform_real_distribution<> chrom_dist;

public:

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
	Manager(unsigned int population_size, unsigned int chromosome_size,
			T max_chromosome_value, T min_chromosome_value, bool use_self_adaptive,
			double mutation_rate, double mutation_change_rate, double similarity_index,
			double crossover_rate, double clonning_rate) : population_size(population_size),
			chromosome_size(chromosome_size), max_chromosome_value(max_chromosome_value),
			min_chromosome_value(min_chromosome_value), use_self_adaptive(use_self_adaptive),
			mutation_rate(mutation_rate), mutation_change_rate(mutation_change_rate),
			similarity_index(similarity_index), clonning_rate(clonning_rate),
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
	Manager(unsigned int population_size, unsigned int chromosome_size,
				T max_chromosome_value, T min_chromosome_value, double mutation_rate,
				double crossover_rate, double clonning_rate) : population_size(population_size),
				chromosome_size(chromosome_size), max_chromosome_value(max_chromosome_value),
				min_chromosome_value(min_chromosome_value), use_self_adaptive(false),
				mutation_rate(mutation_rate), mutation_change_rate(0),
				similarity_index(0), clonning_rate(clonning_rate),
				crossover_rate(crossover_rate) {

		population = std::vector<Chromosome<T > >(population_size);
		initialize(); 

	}

	void applyGeneration() {
		// apply fitness function for each chromosome

		breed(); 
	}

	void breed() {
		std::vector<Chromosome<T > > new_population;

		// Iterate through the chromosomes
		while(new_population.size() < population_size) {	
			// Using a specific selection (eg roulette wheel without replacement) to pick a chromosome
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			int selected_operation = 0;
			int selected_chromosome = getRandChromosome(); 
			if(selected_operation == 0) {
				// Mutation
				population[selected_chromosome].mutation(new_population, min_chromosome_value, max_chromosome_value);
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


		if(use_self_adaptive) {
			selfAdapt();
		}
	}

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
private:
	void initialize() {
		// Create the random objects that will be used
		std::random_device rd;
		rand_engine = std::mt19937 (rd());
		op_dist = std::uniform_real_distribution<> (0, 100);
		chrom_dist = std::uniform_real_distribution<> (0, population_size-1);
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
		unsigned int val = index;

		while(val == index) {
			val = getRandChromosome();	
		}
	}
};


#endif /* MANAGER_HPP_ */

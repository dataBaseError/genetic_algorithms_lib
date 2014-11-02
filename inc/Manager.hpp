
#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include <vector>

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

	}

	void breed() {

		// Iterate through the chromosomes
			// Using a specific selection (eg roulette wheel without replacement) to pick a chromosome
			// Use a random number between to identify which operation to apply (each operation gets a slice of the range)
			// mutation
			// crossover
				// You use the specific selection to pick the next chromosome
			// clonning
		// This produces the population for the next population
	}

	void calculateSimilarity() {

	}

	// Instead of taking a fitness function it might be better to take a function pointer and call that function for the fitness function
	// void *fitness_function(std::vector<Chromosome<T > > population);

	/*static void *fitness_function(std::vector<Chromosome<T > > population) {

		return NULL;
	}*/

};


#endif /* MANAGER_HPP_ */

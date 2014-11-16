#include <cstdio>
#include <cassert>
#include <iostream>     // std::cout
#include <algorithm>    // std::swap_ranges
#include <vector>       // std::vector
#include <time.h>
#include <stdlib.h>
#include <cmath>

#include "Chromosome.hpp"
#include "Manager.hpp"
#include "RouletteWheel.hpp"
#include "SafeQueue.hpp"

/*
bool testChromosomeCreation() {

	Chromosome<int > population(100);
	population.set(0, 100);
	assert(population[0] == 100);
	std::cout << "Passed" << std::endl;
	return true;
}*/
std::pair<double, bool > calculate(Chromosome<unsigned int > chromosome);

/**
 * Test create chromosome of type int
 */
void testChromosomeCreation_uint() {

	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;
	std::vector<Chromosome<unsigned int > > children;
	std::vector<unsigned int > rand_chrom = {2,5,6,1,3,5,4,2};

	// Uses the default constructor to make all the ints.
	Chromosome<unsigned int> chromosome(chromo_size); // Creates {0,0,0,0,0,0,0,0,0,0}
	Chromosome<unsigned int> chromosome2(rand_chrom);
	Chromosome<unsigned int>::initialize(chromo_size, min_value, max_value);
	chromosome.clonning(children);

	// Check that the child is actually in the children
	assert(children.size() == 1);
	std::cout << "Passed child size increased" << std::endl;

	// Check that the clone is equal to the parent
	assert(chromosome == children[0]);
	std::cout << "Passed parent equal to clone" << std::endl;

	chromosome.clonning(children);
	children.back().mutate();

	// Check that the child is actually in the children
	assert(children.size() == 2);
	std::cout << "Passed child size increased" << std::endl;

	// Check that the mutant is equal to the parent
	if(chromosome != children[1]) {
		std::cout << "Passed parent not equal to mutant" << std::endl;
	} else {
		// Note this does not necessary mean the mutate function isnt working.
		// (since it will pick a random number within the range, it my pick the same number and still be accurate).
		std::cout << "Failed parent equal to mutant" << std::endl;
	}
	for (unsigned int i = 0; i < 8; i++) {
		std::cout << children[1][i];
		if(i +1 < 8) {
			std::cout << ",";
		}
	}
	std::cout << '\n';

	chromosome.crossover(chromosome2, children);

	// Check that the child is actually in the children
	assert(children.size() == 4);
	std::cout << "Passed child size increased" << std::endl;

	// Check that the crossover1 is equal to the parent
	assert(chromosome != children[2]);
	std::cout << "Passed parent not equal to crossover1" << std::endl;

	for (unsigned int i = 0; i < 8; i++) {
		std::cout << children[2][i];
		if(i +1 < 8) {
			std::cout << ",";
		}

	}
	std::cout << '\n';

	for (unsigned int i = 0; i < 8; i++) {
		std::cout << children[3][i];
		if(i +1 < 8) {
			std::cout << ",";
		}
	}
	std::cout << '\n';
	// Check that the crossover2 is equal to the parent
	assert(chromosome != children[3]);
	std::cout << "Passed parent not equal to crossover2" << std::endl;

	unsigned int pop_size = 10;
	std::vector<Chromosome<int > > population;
	Chromosome<int >::initialize(chromo_size, min_value, max_value);
	Chromosome<int >::initPopulation(population, pop_size, chromo_size);

	// Check that the population is initialized to the correct size.
	assert(population.size() == pop_size);

	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << population[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	std::cout << "All Chromosome<int> Tests Passed" << std::endl;

}

void testSelection_uint() {
	unsigned int pop_size = 10;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;

	unsigned int max_gen = 100;
	bool use_self_adaptive = false;
	double mutation_rate = 0.1;
	double mutation_change_rate = 0.1;
	double similarity_index = 0.1;
	double crossover_rate = 0.4;
	//double clonning_rate = 0.5;

	unsigned int num_threads = 1;

	// Create a test population of 10
	std::vector<Chromosome<unsigned int > > pop(pop_size);

	// Init the chromosome operations
	Chromosome<unsigned int >::initialize(chromo_size, min_value, max_value);
	// Create a random chromosome population
	Chromosome<unsigned int >::initPopulation(pop, pop_size, chromo_size);

	std::cout << "Init pop = " << std::endl;
	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << pop[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	std::vector<std::pair<unsigned int, double > > fitness(pop_size);

	// Create a set of fitness values
	std::vector<double > fitness_values = { 0.1, 0.4, 0.2, 0.6, 0.7, 0.8, 0.5, 0.75, 0.92, 0.8 };

	for(unsigned int i = 0; i < fitness.size(); i++) {
		fitness[i] = std::pair<unsigned int, double >(i, fitness_values[i]);
	}

	RouletteWheel rw;

	rw.init(fitness);

	unsigned int count = 0;
	while (count < pop_size) {
		Chromosome<unsigned int > cur = pop[rw.next()];
		std::cout << "Next Chromosome = ";
		for (unsigned int i = 0; i < chromo_size; i++) {
			std::cout << cur[i];
			if (i+1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << std::endl;
		count++;
	}

	// TODO test Manager::breed
	Manager<unsigned int > manager(pop_size, chromo_size, max_gen,
					max_value, min_value, use_self_adaptive,
					mutation_rate, mutation_change_rate, similarity_index,
					crossover_rate, num_threads);

	manager.initPopulation();

	std::cout << "Init the chromosomes" << std::endl;

	std::vector<Chromosome<unsigned int > > init_pop = manager.getPopulation();
	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << init_pop[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	manager.breed(fitness);

	std::cout << "Breed the chromosomes" << std::endl;

	std::vector<Chromosome<unsigned int > > breed_pop = manager.getPopulation();
	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << breed_pop[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	std::vector<unsigned int > regTwo = {0,2,4,6,1,3,5,7}; // Not solution (1 collisions)
	// solution {7,3,0,2,5,1,6,4};
	std::vector<unsigned int > regOne = {10,11,12,13,14,15,16,17};
	Chromosome<unsigned int > first(regOne);
	Chromosome<unsigned int > second(regTwo);
	std::vector<Chromosome<unsigned int > >child;

	first.crossover(second, child);

	std::cout << "CHROMOSOME CROSSOVER" << std::endl;
	for (unsigned int j = 0; j < chromo_size; j++) {
		std::cout << first[j];
		if(j +1 < chromo_size) {
			std::cout << ",";
		}
	}
	std::cout << '\n';

	for (unsigned int j = 0; j < chromo_size; j++) {
		std::cout << second[j];
		if(j +1 < chromo_size) {
			std::cout << ",";
		}
	}
	std::cout << '\n';

	std::cout << "RESULT" << std::endl;

	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << child[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	//std::cout << "Fitness for regTwo = " << calculate(regTwo) << std::endl;
}

void testManager_uint() {

	unsigned int pop_size = 50;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;

	unsigned int max_gen = 100;
	bool use_self_adaptive = false;
	double mutation_rate = 0.1;
	double mutation_change_rate = 0.1;
	double similarity_index = 0.1;
	double crossover_rate = 0.4;

	unsigned int num_threads = 10;

	Manager<unsigned int > manager(pop_size, chromo_size, max_gen,
				max_value, min_value, use_self_adaptive,
				mutation_rate, mutation_change_rate, similarity_index,
				crossover_rate, num_threads);

	manager.initPopulation();

	std::cout << "Initial Population " << std::endl;
	std::vector<Chromosome<unsigned int > > initial_pop = manager.getPopulation();
	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << initial_pop[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}

	//test Manager::run after calling for a single generation
	manager.run(&calculate);

	std::cout << "Result Population: " << std::endl;
	std::vector<Chromosome<unsigned int > > final_pop = manager.getPopulation();
	for (unsigned int i = 0; i < pop_size; i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << final_pop[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}
}

std::pair<double, bool > calculate(Chromosome<unsigned int > chromosome)
{
	unsigned int numCollisions = 0;

	//int numCollisions = 0;
	for (int i = 0; i < chromosome.size(); ++i)
	{
		/* Wrap around the genes in the chromosome to check each one */
		for (int j = (i + 1) % chromosome.size(); j != i; ++j, j %= chromosome.size())
		{
			/* Check for vertical collision */
			if (chromosome[i] == chromosome[j])
			{
				++numCollisions;
			}

			/* Check for diagnoal collision, they have a collision if their slope is 1 */
			int Yi = chromosome[i];
			int Yj = chromosome[j];

			if (fabs((double) (i - j) / (Yi - Yj)) == 1.0)
			{
				++numCollisions;
			}
		}
	}

	/* Return the base case of 1, to prevent divide by zero if NO collisions occur */
	if (numCollisions == 0)
	{
		numCollisions= 1;
	}

	// TODO search for results
	// This might be best done in a
	double result = 1.0 / numCollisions;

	return std::pair<double, bool >(result, result == 1);
}

int main(int argc, char **argv) {

	//Skip program name if any
	argc -= (argc > 0);
	argv += (argc > 0);

	testChromosomeCreation_uint();

	std::cout << std::endl;

	testSelection_uint();

	std::cout << std::endl;

	testManager_uint();

	return 0;
}

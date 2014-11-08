#include <cstdio>
#include <cassert>
#include <iostream>     // std::cout
#include <algorithm>    // std::swap_ranges
#include <vector>       // std::vector
#include <time.h>
#include <stdlib.h>

#include "Chromosome.hpp"
#include "Manager.hpp"
#include "RouletteWheel.hpp"

/*
bool testChromosomeCreation() {

	Chromosome<int > population(100);
	population.set(0, 100);
	assert(population[0] == 100);
	std::cout << "Passed" << std::endl;
	return true;
}*/

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
	Chromosome<unsigned int> chromosome2(rand_chrom); // Creates {0,0,0,0,0,0,0,0,0,0}
	Chromosome<unsigned int>::initialize(chromo_size, min_value, max_value);
	chromosome.clonning(children);

	// Check that the child is actually in the children
	assert(children.size() == 1);
	std::cout << "Passed child size increased" << std::endl;

	// Check that the clone is equal to the parent
	assert(chromosome == children[0]);
	std::cout << "Passed parent equal to clone" << std::endl;

	chromosome.mutate(children);

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

	// Check that the crossover2 is equal to the parent
	assert(chromosome != children[3]);
	std::cout << "Passed parent not equal to crossover2" << std::endl;

	for (unsigned int i = 0; i < 8; i++) {
		std::cout << children[3][i];
		if(i +1 < 8) {
			std::cout << ",";
		}
	}
	std::cout << '\n';

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

void testManager_uint() {
	unsigned int pop_size = 10;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;

	unsigned int max_gen = 100;
	bool use_self_adaptive = true;
	double mutation_rate = 0.1;
	double mutation_change_rate = 0.1;
	double similarity_index = 0.1;
	double crossover_rate = 0.4;
	double clonning_rate = 0.5;

	Manager<unsigned int > manger(pop_size, chromo_size, max_gen,
			max_value, min_value, use_self_adaptive,
			mutation_rate, mutation_change_rate, similarity_index,
			crossover_rate, clonning_rate);

	// Create a test population of 10
	std::vector<Chromosome<unsigned int > > pop(10);

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

	std::vector<std::pair<unsigned int, double > > fitness(10);

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

	 //manager.
}

int main(int argc, char **argv) {

	//Skip program name if any
	argc -= (argc > 0);
	argv += (argc > 0);

	testChromosomeCreation_uint();

	std::cout << std::endl;
	testManager_uint();

	return 0;
}

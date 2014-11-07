#include <cstdio>
#include <cassert>
#include <iostream>     // std::cout
#include <algorithm>    // std::swap_ranges
#include <vector>       // std::vector
#include <time.h>
#include <stdlib.h>

#include "Chromosome.hpp"

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

	Chromosome<unsigned int > val = children.back();
	//for (auto it=(*val).begin(); it!=(*val).end(); ++it)
	//	std::cout << ' ' << *it;
	//std::cout << '\n';

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

	cout << "All Chromosome<int> Tests Passed" << endl;

}

int main(int argc, char **argv) {

	//Skip program name if any
	argc -= (argc > 0);
	argv += (argc > 0);

	testChromosomeCreation_uint();

	return 0;
}

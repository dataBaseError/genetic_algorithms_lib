#include <cstdio>
#include <cassert>
#include <iostream>

#include "Chromosome.hpp"

/*
bool testChromosomeCreation() {

	Chromosome<int > population(100);
	population.set(0, 100);
	assert(population[0] == 100);
	std::cout << "Passed" << std::endl;
	return true;
}*/

int main(int argc, char **argv) {

	//Skip program name if any
	argc -= (argc > 0);
	argv += (argc > 0);

	//testChromosomeCreation();
	Chromosome<int > population(100);
	population.mutate(0.5);

	return 0;
}

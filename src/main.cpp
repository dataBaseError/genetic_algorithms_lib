#include <cstdio>
#include <cassert>
#include <iostream>     // std::cout
#include <algorithm>    // std::swap_ranges
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>

#include "Chromosome.hpp"
#include "Manager.hpp"

/*
bool testChromosomeCreation() {

	Chromosome<int > population(100);
	population.set(0, 100);
	assert(population[0] == 100);
	std::cout << "Passed" << std::endl;
	return true;
}*/
double calculate(Chromosome<unsigned int> chromosome);

void testManager_uint() {

	unsigned int pop_size = 50;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;

	unsigned int max_gen = 1000;
	bool use_self_adaptive = false;
	double mutation_rate = 0.1;
	double mutation_change_rate = 0.1;
	double similarity_index = 0.1;
	double crossover_rate = 0.4;

	unsigned int num_threads = 5;

	Manager<unsigned int > manager(pop_size, chromo_size, max_gen,
				max_value, min_value, use_self_adaptive,
				mutation_rate, mutation_change_rate, similarity_index,
				crossover_rate, num_threads);

	manager.initPopulation();

	std::cout << "Initial Population " << std::endl;
	std::vector<Chromosome<unsigned int> > initial_pop = manager.getPopulation();
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
	std::vector<Chromosome<unsigned int> > final_pop = manager.getPopulation();
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

double calculate(Chromosome<unsigned int> chromosome)
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

	return result;//, result == 1);
}

int main(int argc, char **argv) {

    // EXAMPLES
   // boost::atomic_int producer_count(0);
    //boost::thread t(&testChromosomeCreation_uint);
   // boost::lockfree::queue<int> queue(128);

    //Skip program name if any
	argc -= (argc > 0);
	argv += (argc > 0);

	testManager_uint();

	return 0;
}

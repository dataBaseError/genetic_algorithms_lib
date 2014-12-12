#include <cstdio>
#include <cassert>
#include <iostream>     // std::cout
#include <algorithm>    // std::swap_ranges
#include <string>

#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include "boost/program_options.hpp"

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

/*void testManager_uint() {

	unsigned int pop_size = 50;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = 7;

	unsigned int max_gen = 10000;
	double mutation_rate = 0.1;
	double crossover_rate = 0.4;

	unsigned int num_compeditors = 1;
	unsigned int num_threads = 1;

	Manager<unsigned int > manager(pop_size, chromo_size, max_gen,
				max_value, min_value, mutation_rate, crossover_rate,
				num_compeditors, num_threads);

	/*manager.initPopulation();

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

	}/
	manager.run(&calculate);

	std::vector<Chromosome<unsigned int> > solutions = manager.getSolutions();

	std::cout << "Solutions: " << std::endl;
	for (unsigned int i = 0; i < solutions.size(); i++) {
		for (unsigned int j = 0; j < chromo_size; j++) {
			std::cout << solutions[i][j];
			if(j +1 < chromo_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}
}*/

template <class T>
void measure_performance(std::vector<unsigned int > pop_size, unsigned int chromosome_size,
	T min_value, T max_value, unsigned int max_gen, std::vector<double > mutation_rate,
	std::vector<double > crossover_rate, unsigned int num_compeditors, 
	unsigned int num_threads) {

	Manager<unsigned int > manager(pop_size, chromosome_size, max_gen,
				max_value, min_value, mutation_rate, crossover_rate,
				num_compeditors, num_threads);


	manager.run(&calculate);

	std::vector<Chromosome<unsigned int> > solutions = manager.getSolutions();

	std::cout << "Solutions: " /*<< solutions.size())*/ << std::endl;
	for (unsigned int i = 0; i < solutions.size(); i++) {
		for (unsigned int j = 0; j < chromosome_size; j++) {
			std::cout << solutions[i][j];
			if(j +1 < chromosome_size) {
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

template <class T> 
std::vector<T > parseVector(boost::program_options::variables_map vm, std::string key) {

	std::vector<T > val;
	if (!vm[key].empty()) {
		val = vm[key].as<std::vector<T > >();
		// good to go
		for(unsigned int i = 0; i< val.size(); i++) {
			std::cout << val[i] << std::endl;	
		}
	}
	return val;
}

// Command line interface
// e.g. ../bin/GALibrary --c 1 --t 1 --n 8 --gen 1000 --pop_size 50 60 --m_rate 0.1 0.9 --c_rate 0.4 0.6

int parse_args(int argc, char **argv) {

	namespace po = boost::program_options; 
	po::options_description desc("Allowed options");
	desc.add_options()
		("c", po::value<unsigned int >(), "set the number of competitors")
		("t", po::value<unsigned int >(), "set the number of worker threads for each competitor")
		("n", po::value<unsigned int >(), "the number of queens")
		("gen", po::value<unsigned int >(), "the maximum number of generations")
		("pop_size", po::value<std::vector<unsigned int > >()->multitoken(), "The population values for each competitor")
		("m_rate", po::value<std::vector<double > >()->multitoken(), "The mutation rate for each competitor")
		("c_rate", po::value<std::vector<double > >()->multitoken(), "The crossover rate for each competitor");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	unsigned int num_competitors = vm["c"].as<unsigned int >();
	unsigned int num_threads = vm["t"].as<unsigned int >();
	unsigned int chromo_size = vm["n"].as<unsigned int >();
	unsigned int max_gen = vm["gen"].as<unsigned int >();

	std::vector<unsigned int> pop_size = parseVector<unsigned int >(vm, "pop_size");
	std::vector<double > m_rate = parseVector<double >(vm, "m_rate");
	std::vector<double > c_rate = parseVector<double >(vm, "c_rate");

	if(num_competitors > 0 && pop_size.size() == num_competitors && pop_size.size() == m_rate.size() && pop_size.size() == c_rate.size()) {
		std::cout << "Invalid Input" << std::endl;
		return -1;
	}

	// TODO fix for actual cmd
	unsigned int max_value = chromo_size -1;
	unsigned int min_value = 0;
	measure_performance<unsigned int>(pop_size, chromo_size,
		min_value, max_value, max_gen, m_rate, c_rate, 
		num_competitors, num_threads);
}

int main(int argc, char **argv) {

    // EXAMPLES
   // boost::atomic_int producer_count(0);
    //boost::thread t(&testChromosomeCreation_uint);
   // boost::lockfree::queue<int> queue(128);

    //Skip program name if any
	//argc -= (argc > 0);
	//argv += (argc > 0);

	parse_args(argc, argv);

	//testManager_uint();

	unsigned int num_competitors = 2;

	std::vector<unsigned int > pop_size(num_competitors);
	pop_size[0] = 50;
	//pop_size[1] = 60;


	//unsigned int pop_size = 50;
	unsigned int chromo_size = 8;
	unsigned int min_value = 0;
	unsigned int max_value = chromo_size-1;
	unsigned int max_gen = 10000;

	std::vector<double > mutation_rate(num_competitors);
	mutation_rate[0] = 0.1;
	//mutation_rate[1] = 0.9;

	//double mutation_rate = 0.1;
	std::vector<double > crossover_rate(num_competitors);
	crossover_rate[0] = 0.4;
	//crossover_rate[1] = 0.6;
	//double crossover_rate = 0.4;

	unsigned int num_threads = 5;

	//measure_performance<unsigned int>(pop_size, chromo_size,
		//min_value, max_value, max_gen, mutation_rate, crossover_rate, 
		//num_competitors, num_threads);

	return 0;
}

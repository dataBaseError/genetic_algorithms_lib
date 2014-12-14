/**
 *  The MIT License (MIT)
 *
 * Copyright (c) 2014  Joseph Heron, Jonathan Gillett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

double calculate(Chromosome<unsigned int> chromosome);

template <class T>
int measure_performance(std::vector<unsigned int > pop_size, unsigned int chromosome_size,
	T min_value, T max_value, unsigned int max_gen, std::vector<double > mutation_rate,
	std::vector<double > crossover_rate, unsigned int num_compeditors, 
	unsigned int num_threads) {

	Manager<unsigned int > manager(pop_size, chromosome_size, max_gen,
				max_value, min_value, mutation_rate, crossover_rate,
				num_compeditors, num_threads);


	unsigned int num_gen = manager.run(&calculate);

	std::vector<Chromosome<unsigned int> > solutions = manager.getSolutions();

	if(solutions.size() > 0) {
		std::cout << "1";
	}
	else {
		std::cout << "0";
	}

	std::cout << ", " << num_gen;
	return 0;
	/*std::cout << "Solutions: " << std::endl;
	for (unsigned int i = 0; i < solutions.size(); i++) {
		for (unsigned int j = 0; j < chromosome_size; j++) {
			std::cout << solutions[i][j];
			if(j +1 < chromosome_size) {
				std::cout << ",";
			}
		}
		std::cout << '\n';

	}*/
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

	// This might be best done in a
	double result = 1.0 / numCollisions;

	return result;//, result == 1);
}

template <class T> 
std::vector<T > parseVector(boost::program_options::variables_map vm, std::string key) {

	std::vector<T > val;
	if (!vm[key].empty()) {
		val = vm[key].as<std::vector<T > >();
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

	if(num_competitors <= 0 && pop_size.size() == num_competitors && pop_size.size() == m_rate.size() && pop_size.size() == c_rate.size()) {
		std::cout << "Invalid Input" << std::endl;
		return -1;
	}

	unsigned int max_value = chromo_size -1;
	unsigned int min_value = 0;
	return measure_performance<unsigned int>(pop_size, chromo_size,
		min_value, max_value, max_gen, m_rate, c_rate, 
		num_competitors, num_threads);
}

int main(int argc, char **argv) {

	return parse_args(argc, argv);
}

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

#ifndef COMPETITOR_HPP_
#define COMPETITOR_HPP_

#include <vector>

#include <boost/atomic.hpp>

#include "Chromosome.hpp"
#include "SafeQueue.hpp"
#include "SafeVector.hpp"

#include <boost/thread/barrier.hpp>

template <class T>
class Competitor {

	unsigned int population_size;
	double mutation_rate;

	double crossover_rate;

public:

	SafeVector<Chromosome<T > > population;
	SafeQueue<Result > result_queue;

	// This should not need to be a safe vector since access will be syncronized by other mechinsims
	SafeVector<Result >fitness_results;

	Competitor(unsigned int population_size, double mutation_rate, double
		crossover_rate) : population_size(population_size),
		mutation_rate(mutation_rate), crossover_rate(crossover_rate) {
	}

	~Competitor() {

	}

	unsigned int getPopulationSize() {
		return this->population_size;
	}

	double getMutationRate() {
		return this->mutation_rate;
	}

	double getCrossoverRate() {
		return this->crossover_rate;
	}

	void initPopulation(unsigned int chromosome_size) {
		std::vector<Chromosome<T > > pop;
		Chromosome<T >::initPopulation(pop, this->population_size, chromosome_size);
		this->population.push_back(pop);
	}

};


#endif /* COMPETITOR_HPP_ */

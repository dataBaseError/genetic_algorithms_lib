
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

	//boost::atomic<bool> waiting;

public:

	//boost::atomic<bool> done;
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


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

	// done, waiting, m_cond, population, results_queue, population_size, mutation_rate, crossover_rate

	unsigned int population_size;
	double mutation_rate;

	double crossover_rate;

	boost::atomic<bool> waiting;

	boost::condition_variable m_cond;
	boost::mutex mtx_;

	boost::barrier breed_wall;

public:

	boost::atomic<bool> done;
	SafeVector<Chromosome<T > > population;
	SafeQueue<Result > result_queue;

	// This should not need to be a safe vector since access will be syncronized by other mechinsims
	SafeVector<Result >fitness_results;

	// Will define number_of_competitors which will have a number of worker threads and the following values set for them:
	// done, waiting, m_cond, population, results_queue, population_size, mutation_rate, crossover_rate

	Competitor(unsigned int population_size, double mutation_rate, double
		crossover_rate, unsigned int num_workers) : population_size(population_size),
		mutation_rate(mutation_rate), crossover_rate(crossover_rate),
		done(false), waiting(false), breed_wall(num_workers) {
	}

	~Competitor() {

	}

	bool wait_for_work() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		if(!done) {
			if(!waiting) {
				m_cond.wait(lock);
			}			
		}
		else {
			return true;
		}
		return false;
	}

	void set_waiting(bool val){
		boost::unique_lock<boost::mutex> guard(mtx_);
		waiting = val;
	}

	void start_working() {
		m_cond.notify_all();
	}

	unsigned int getPopulationSize() {
		return this->population_size;
	}

	void initPopulation(unsigned int chromosome_size) {
		std::vector<Chromosome<T > > pop;
		Chromosome<T >::initPopulation(pop, this->population_size, chromosome_size);
		this->population.push_back(pop);
	}

	void wait() {
		this->breed_wall.wait();
	}

};


#endif /* COMPETITOR_HPP_ */

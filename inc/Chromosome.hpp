
#ifndef CHROMOSOME_HPP_
#define CHROMOSOME_HPP_

#include <time.h>
#include <stdlib.h>
#include <vector>

template <class T>
class Chromosome {

protected:
    std::vector<T > chromosome;

public:

    Chromosome(int chromosome_size) : chromosome(chromosome_size){

    }

    void mutate(double mutation_rate) {
    	// Apply mutation operation to the chromosome
    }

    void crossover(double crossover_rate, Chromosome<T > &other) {
    	// Apply crossover operation to the chromosome
    }

    //void

    /*T operator[] (const unsigned int i) {
    	return this->chromosomes[i];
    }

    void set (const unsigned int i, T value) {
    	this->chromosomes[i] = value;
    }*/

};

#endif /* CHROMOSOME_HPP_ */

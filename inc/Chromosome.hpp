
#ifndef CHROMOSOME_HPP_
#define CHROMOSOME_HPP_

#include <time.h>
#include <stdlib.h>
#include <vector>

// TODO define this when the library is initialized
//#define CHROMOSOME_SIZE 8

template <class T>
class Chromosome {

	//int CHROMOSOME_SIZE;

protected:

    std::vector<T > chromosomes;

public:

    Chromosome(int chromosome_size) : chromosomes(chromosome_size){

    }

    T operator[] (const unsigned int i) {
    	return this->chromosomes[i];
    }

    void set (const unsigned int i, T value) {
    	this->chromosomes[i] = value;
    }

};

#endif /* CHROMOSOME_HPP_ */

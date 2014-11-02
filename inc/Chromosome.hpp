
#ifndef CHROMOSOME_HPP_
#define CHROMOSOME_HPP_

#include <time.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <random>

#define MINIMUM_NUMBER 0

template <class T>
class Chromosome {

protected:
    std::vector<T > chromosome;

    // random number generator values.
    static std::mt19937 random_engine;
    static std::uniform_real_distribution<> rand_chrom_elem;
    static std::uniform_real_distribution<> rand_value;


public:

    Chromosome(unsigned int chromosome_size) : chromosome(chromosome_size){
    }

    static void initialize(unsigned int chromosome_size, T min_chromosome_value, T max_chromosome_value) {
    	
	// Set up the random number generator engines.
	std::random_device rd;
	random_engine = std::mt19937 (rd());
	rand_chrom_elem = std::uniform_real_distribution<> (MINIMUM_NUMBER, chromosome_size);
	rand_value = std::uniform_real_distribution<> (min_chromosome_value, max_chromosome_value);
    }

    void mutate(std::vector<Chromosome<T > > &children, T min_chromosome_value, T max_chromosome_value) {
    	clonning(children);

    	// Identify element that will be changed
    	int mutated_index = getRandomElement();

    	// Apply mutation operation to the chromosome
    	if(std::is_same<T, bool>::value) {
    		// Flip the bit
    		children.back()[mutated_index] = !children.back()[mutated_index];
    	} else { 
		// This will really only work for 'primitive types'
    		// Choose a random number within the range
    		children.back()[mutated_index] = rand_value(random_engine);
    	}
    }

    void crossover(Chromosome<T > &other, std::vector<Chromosome<T > > &children) {
    	// Copy value to the next population
    	clonning(children);
	// Copy other to the next population
    	clonning(children, other);

    	// Randomly pick one point (where the cross over starts)
    	int crossover_index = getRandomElement();

    	// Apply crossover operation to the chromosome

    	// Using the index inclusive approach
    	// First gets l1 + r2 and second gets l2 + r1
    	std::swap_ranges((*(children.end()-2)).begin(), (*(children.end()-2)).begin()+crossover_index, (*(children.end()-1)).rbegin()+crossover_index+1);

    	// Alternative exclusive index approach
    	//std::swap_ranges((*(children.end()-2)).begin(), (*(children.end()-2)).begin()+crossover_index+1, (*(children.end()-1)).rbegin()+crossover_index);

    }

    void clonning(std::vector<Chromosome<T > > &children) {
    	// Copy value to the next population
    	children.push_back(chromosome);
    }

private:
    void clonning(std::vector<Chromosome<T > > &children, Chromosome<T > &chromosome) {
	// Copy value to the next population
	children.push_back(chromosome);
    }

    unsigned int getRandomElement() {
    	return rand_chrom_elem(random_engine);
    }

};

#endif /* CHROMOSOME_HPP_ */

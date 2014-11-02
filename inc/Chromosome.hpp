
#ifndef CHROMOSOME_HPP_
#define CHROMOSOME_HPP_

#include <time.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

template <class T>
class Chromosome {

protected:
    std::vector<T > chromosome;

public:

    Chromosome(int chromosome_size) : chromosome(chromosome_size){

    }

    void initialize() {
    	// Initialize the chromosome.
    }

    void mutate(double mutation_rate, std::vector<Chromosome<T > > &children, T min_chromosome_value, T max_chromosome_value) {
    	clonning(children);

    	// Identify element that will be changed
    	int mutated_index = getRandomElement();

    	// Apply mutation operation to the chromosome
    	if(std::is_same<T, bool>::value) {
    		// Flip the bit
    		children.back()[mutated_index] != children.back()[mutated_index];
    	} else {
    		// Choose a random number within the range
    		children.back()[mutated_index] = rand() % max_chromosome_value + min_chromosome_value;
    	}
    }

    void crossover(double crossover_rate, Chromosome<T > &other, std::vector<Chromosome<T > > &children) {
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

    void clonning(std::vector<Chromosome<T > > &children, Chromosome<T > &chromosome) {
		// Copy value to the next population
		children.push_back(chromosome);
	}

    int getRandomElement() {
    	return rand() % chromosome.size()-1;
    }

};

#endif /* CHROMOSOME_HPP_ */

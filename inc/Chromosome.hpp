
#ifndef CHROMOSOME_HPP_
#define CHROMOSOME_HPP_

#include <iostream>

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
    static std::uniform_int_distribution<int> rand_chrom_elem;
    // This only works with integer values
    static std::uniform_int_distribution<int > rand_value;


public:
    
    /**
     * Create a chromosome of the given size
     * @param chromosome_size The size of the chromosome.
     */
    Chromosome(unsigned int chromosome_size) : chromosome(chromosome_size){
    }

    ~Chromosome() {

    }

    /**
     * Create a chromosome of the given size
     * @param chromosome_size The size of the chromosome.
     */
    Chromosome(std::vector<T > chromosome) {
    	this->chromosome = chromosome;
    }

    void randChromosome() {
    	std::cout << chromosome.size() << std::endl;
		for(unsigned int i = 0; i < chromosome.size(); i++) {
			chromosome[i] = getRandomValue();
		}
    }

    static void initPopulation(std::vector<Chromosome<T > > &population,
    		unsigned int population_size, unsigned int chromosome_size) {
    	population.clear();
    	for(unsigned int i = 0; i < population_size; i++) {
    		Chromosome<T > chrom (chromosome_size);
    		chrom.randChromosome();
    		population.push_back(chrom);
    	}
    }

    /**
     * Set up the random number generator engines to allow within the defined ranges.
     * @param chromosome_size The size of the chromosome.
     * @param min_chromosome_value The minimum value of a chromosome element.
     * @param max_chromosome_value The maximum value of a chromosome element.
     */
    static void initialize(unsigned int chromosome_size, T min_chromosome_value, T max_chromosome_value) {

		// Set up the random number generator engines.
		std::random_device rd;
		random_engine = std::mt19937 (rd());
		rand_chrom_elem = std::uniform_int_distribution<int> (MINIMUM_NUMBER, chromosome_size - 1);
		rand_value = std::uniform_int_distribution<int > (min_chromosome_value, max_chromosome_value);
    }

    /**
     * Apply the mutation operation to the chromosome and put it into the next generation.
     * @param children The next generation of chromosomes (the next population) that is currently being bread.
     */
    void mutate(std::vector<Chromosome<T > > &children) {
    	clonning(children);

    	// Identify element that will be changed
    	int mutated_index = getRandomElement();

        // Mutate the element
    	mutateElement(mutated_index);
    }

    /**
     * Apply the crossover operation to the chromosome and the other chromosome provided.
     * This will produce two offspring that are placed in the next generation.
     * *Note, Assumed that the two chromosomes (this and other) are of the same length.
     * @param other The second chromosome involved in the crossover operation
     * @param children The next generation of the population.
     * 
     * Note that there are two possible methods to apply this crossover: index inclusive and index exclusive.
     * Currently index inclusive is used, this is where the site where the chromosomes are crossed over is
     * included in the crossover. Exclusive is the opposite where the index where the chromosomes are
     * crossed over is not included in the crossover.
     * 
     * The crossover will work as:
     * chromosome1 = l1 + r1
     * chromosome2 = l2 + r2
     * after the crossover:
     * chromosome1 = l1 + r2
     * chromosome2 = l2 + r1
     * Where l1 and l2 are subsets of the chromosome = chromosome[0..crossover_site-1] for each respective chromosome
     * and r1 and r2 are subsets of the chromosome = chromosome[crossover_site..chromosome.size()-1] for each respective chromosome.
     */
    void crossover(Chromosome<T > &other, std::vector<Chromosome<T > > &children) {
    	// Copy value to the next population
    	clonning(children);
    	// Copy other to the next population
    	other.clonning(children);

    	// Randomly pick one point (where the cross over starts)
    	int crossover_index = getRandomElement(0);

    	// Using the index inclusive approach
    	// First gets l1 + r2 and second gets l2 + r1
	// Note this approach does not work if crossover_index == 0
    	std::swap_ranges((*(children.end()-2)).chromosome.begin(), (*(children.end()-2)).chromosome.begin()+crossover_index, (*(children.end()-1)).chromosome.rbegin()+(chromosome.size()-1-crossover_index)+1);

    	// Alternative exclusive index approach
	// Note this approach does not work if crossover_index == 7
    	//std::swap_ranges((*(children.end()-2)).begin(), (*(children.end()-2)).begin()+crossover_index+1, (*(children.end()-1)).rbegin()+(chromosome.size()-1-crossover_index));

    }

    /**
     * Copy the chromosome to the next generation.
     * @param children The next generation.
     */
    void clonning(std::vector<Chromosome<T > > &children) {
    	// Copy value to the next population
    	children.push_back(*this);
    }

    /**
     * Overload the array operator.
     * @param n The index to retrieve the value at.
     * @return The value.
     */
    T operator[](unsigned int n) const {
    	return *this->chromosome[n];
    }

    /**
     * Overload the array operator
     * @param n The index to retrieve the value at.
     * @return The reference to the value.
     */
    T& operator[](unsigned int n) {
		return this->chromosome[n];
	}

    /**
     * Overload the == operation.
     * @param other The Chromosome to compare too.
     * @return Whether the two chromosomes are the same.
     */
    inline bool operator==(const Chromosome<T >& other){ return this->chromosome == other.chromosome; }

    /**
     * Overload the != operation, makes use of == operator overload
     * @param other The chromosome to check equality.
     * @return Whether the two chromosomes are different.
     */
    inline bool operator!=(const Chromosome<T >& other){ return !(*this == other); }

private:

    /**
     * Get a random index to allow for the retrieval a random element within the chromosome.
     * @return The random index.
     */
    unsigned int getRandomElement() {
    	return rand_chrom_elem(random_engine);
    }

	/**
	 * Get a random number between 0 and chromosome.size()-1 that is not index
	 * @param index The only value within the range that the return cannot be.
	 * @return A number within the defined range that is not index.
	 */
	unsigned int getRandomElement(unsigned int index) {
		unsigned int val = getRandomElement();

		while(val == index) {
			val = getRandomElement();
		}
		return val;
	}

    void mutateElement(unsigned int mutated_index) {
    	// Apply mutation operation to the chromosome
    	if(std::is_same<T, bool>::value) {
    		// Flip the bit
    		this->chromosome[mutated_index] = !this->chromosome[mutated_index];
    	} else { 
		// This will really only work for 'primitive types'
    		// Choose a random number within the range
    		this->chromosome[mutated_index] = getRandomValue(this->chromosome[mutated_index]);
    	}
    }

    T getRandomValue() {
    	return rand_value(random_engine);
    }

    T getRandomValue(T prev) {
        T val = getRandomValue();

        while(val == prev) {
            val = getRandomValue();
        }
        return val;
    }

};

template<class T>
std::mt19937 Chromosome<T >::random_engine;
template<class T>
std::uniform_int_distribution<int > Chromosome<T >::rand_chrom_elem;
template<class T>
std::uniform_int_distribution<int > Chromosome<T >::rand_value;

#endif /* CHROMOSOME_HPP_ */

#include "Chromosome.hpp"

//Chromosome::Chromosome(int chromosome_size)
/*template <typename T>
Chromosome<T>::Chromosome(int chromosome_size) : chromosomes(chromosome_size) {

}*/

template <typename T>
Chromosome<T> operator[] (const unsigned int i) {
	return this->chromosomes[i];
}


void init() {
    srand(time(NULL));
    
}

void mutate(chromosome& chromo) {
    // Apply the mutation operation
}

void crossover(chromosome& chromo1, chromosome& chromo2) {

}

void inbreeding(bool inbreed) {
    int r = min_chromosome_value + (int)(max_chromosome_value * rand() / (RAND_MAX + 1.0));
    //1 + (int)( 10.0 * rand() / ( RAND_MAX + 1.0 ) )
}

void calcSimilarity(chromosome* chromo) {
    
}

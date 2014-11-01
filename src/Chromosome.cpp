#include "Chromosome.hpp"

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

// List of constants that are required about the problem:
const int size_of_chromosome = 8;
const double max_chromosome_value = 7.0;
const int min_chromosome_value = 0;

const double mutation_rate_initial = 0.1;

const double crossover_rate = 0.7;
const double clonning_rate = 0.3;

const double mutation_change_threshold = 1.0;
const double similarity_index = 0.15;

const int population_size = 64;

#include <inc/chromosome.h>

/*int main(int argc, char *argv[]) {
    
    //Skip program name if any
    //argc -= (argc > 0);
    //argv += (argc > 0);

    //const int chromosome_size = (argv[0]);
}*/

int process_generation() {

    // 1. Take given chromosomes
    // 2. Apply ga operators
    // 3. calculate similarity
        // update mutation rate
    // return results
}
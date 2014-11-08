
#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include <vector>
#include <utility>
//#include "Chromosome.hpp"
//#include "SimpleChromosome.hpp"

//using namespace std;

/**
 * Selection interface, defines the operations that all selection method
 * implementations such as roulette wheel selection must provide.
 */
class Selection
{
public:
    /**
     * Initialize the selection iterator with the fitness distribution of the
     * chromosomes which is used to generate the selection range of chromosomes
     * for the selection method.
     *
     * @param fitness The fitness distribution for the chromosomes to be used
     * for the selection.
     */
    virtual void init(std::vector<std::pair<unsigned int, double> > &fitness) = 0;

    /**
     * The iterator method which uses the selection method to get the next
     * chromosome to be used for the genetic algorithm.
     *
     * @return The next chromosome selected.
     */
    virtual unsigned int next() = 0;

    virtual ~Selection() {}
};


#endif /* SELECTION_HPP_ */

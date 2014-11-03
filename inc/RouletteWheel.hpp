/*
 * RouletteWheel.hpp
 *
 *  Created on: Nov 3, 2014
 *      Author: jon
 */

#ifndef ROULETTEWHEEL_HPP_
#define ROULETTEWHEEL_HPP_

#include "Selection.hpp"

class RouletteWheel : public Selection
{
public:
    /**
     * Initialize the selection iterator with the fitness distribution of the
     * chromosomes which is used to generate the selection range of chromosomes
     * for the selection method.
     *
     * @param fitness The fitness distribution for the chromosomes to be used
     * for the roulette wheel selection.
     */
    virtual void init(vector<pair<SimpleChromosome, double> > &fitness);

    /**
     * The iterator method which uses roulette wheel selection to get the next
     * chromosome to be used for the genetic algorithm.
     *
     * @return The next chromosome selected.
     */
    virtual SimpleChromosome next();
};

#endif /* ROULETTEWHEEL_HPP_ */

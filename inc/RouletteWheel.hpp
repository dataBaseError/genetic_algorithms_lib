
#ifndef ROULETTEWHEEL_HPP_
#define ROULETTEWHEEL_HPP_

#include <random>
#include <map>
#include "Selection.hpp"

class RouletteWheel : public Selection
{
private:
    mt19937 engine;
    uniform_real_distribution<double> distribution;
    const double EPSILON = 1.0E-15;
    double left = 0.0;
    double right = 0.0 + EPSILON;
    map<pair<double, double>, SimpleChromosome> selection;

public:
    /**
     * Default constructor for roulette wheel selection method, initializes
     * the random engine used for selection process.
     */
    RouletteWheel();

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


#ifndef ROULETTEWHEEL_HPP_
#define ROULETTEWHEEL_HPP_

#include <random>
#include <map>

#include "Selection.hpp"
#include "Chromosome.hpp"

using namespace std;

template <class T>
class RouletteWheel : public Selection<T >
{
private:
    mt19937 engine;
    uniform_real_distribution<double> distribution;
    const double EPSILON = 1.0E-15;
    double left = 0.0;
    double right = 0.0 + EPSILON;
    map<pair<double, double>, Chromosome<T > > selection;

public:
    /**
     * Default constructor for roulette wheel selection method, initializes
     * the random engine used for selection process.
     */
    RouletteWheel()
	{
		random_device rd;
		this->engine = mt19937(rd());
	}

    virtual ~RouletteWheel()
	{

	}

    /**
     * Initialize the selection iterator with the fitness distribution of the
     * chromosomes which is used to generate the selection range of chromosomes
     * for the selection method.
     *
     * @param fitness The fitness distribution for the chromosomes to be used
     * for the roulette wheel selection.
     */
    virtual void init(vector<pair<Chromosome<T >, double> > &fitness)
    {
    	double lower = this->left;
    	double upper = this->right;

    	// Calculate the right bound
    	for (auto it = fitness.begin(); it != fitness.end(); ++it)
    	{
    		this->right += it->second + this->EPSILON;
    	}

    	// Initialize the uniform distribution random generator
    	this->distribution = uniform_real_distribution<double>(this->left, this->right);

    	// Add each chromosome and interval to roulette wheel selection
    	for (auto it = fitness.begin(); it != fitness.end(); ++it)
    	{
    		upper = lower + it->second;
    		this->selection.insert({make_pair(lower, upper), it->first});
    		lower = upper + this->EPSILON;
    	}
    }

    /**
     * The iterator method which uses roulette wheel selection to get the next
     * chromosome to be used for the genetic algorithm.
     *
     * @return The next chromosome selected.
     */
    virtual Chromosome<T >& next()
	{
		double rand_num = this->distribution(this->engine);

		// Find the chromosome where the random number is in the interval
		for (auto it = this->selection.begin(); it != this->selection.end(); ++it)
		{
			if (rand_num >= it->first.first && rand_num < it->first.second)
			{
				return it->second;
			}
		}

		// TODO fatal error due to floating point arithmetic if no match found!
		Chromosome<T > chromo = Chromosome<T >();
		return chromo;
	}
};

#endif /* ROULETTEWHEEL_HPP_ */

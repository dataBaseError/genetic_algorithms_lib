#include "../inc/RouletteWheel.hpp"

/*
RouletteWheel::RouletteWheel()
{
    random_device rd;
    this->engine = mt19937(rd());
}*/

/*template<class T >
void RouletteWheel::init(vector<pair<Chromosome<T >, double> > &fitness)
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
}*/

/*
template<class T >
Chromosome<T > RouletteWheel::next()
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
}*/

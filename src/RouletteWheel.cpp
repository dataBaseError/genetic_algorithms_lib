#include "RouletteWheel.hpp"


RouletteWheel::RouletteWheel()
{
    std::random_device rd;
    this->engine = std::mt19937(rd());
}

RouletteWheel::~RouletteWheel() {

}

void RouletteWheel::init(std::vector<std::pair<unsigned int, double> > &fitness)
{
	double lower = this->left;
	double upper = this->right;

	// Calculate the right bound
	for (auto it = fitness.begin(); it != fitness.end(); ++it)
	{
		this->right += it->second + this->EPSILON;
	}

	// Initialize the uniform distribution random generator
	this->distribution = std::uniform_real_distribution<double>(this->left, this->right);

	// Add each chromosome and interval to roulette wheel selection
	for (auto it = fitness.begin(); it != fitness.end(); ++it)
	{
		upper = lower + it->second;
		this->selection.insert({std::make_pair(lower, upper), it->first});
		lower = upper + this->EPSILON;
	}
}

unsigned int RouletteWheel::next()
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
	//unsigned int bad_return = 0;
	return 0;
}

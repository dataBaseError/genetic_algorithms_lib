#include "RouletteWheel.hpp"


RouletteWheel::RouletteWheel()
{
    std::random_device rd;
    this->engine = std::mt19937(rd());
}

RouletteWheel::~RouletteWheel() {

}

void RouletteWheel::init(std::vector<Result > &fitness)
{
	// Ensure the selection map is empty.
	selection.clear();

	double lower = this->left;
	double upper = this->right;

	// Calculate the right bound
	for (auto it = fitness.begin(); it != fitness.end(); ++it)
	{
		this->right += it->getResult() + this->EPSILON;
	}

	// Initialize the uniform distribution random generator
	this->distribution = std::uniform_real_distribution<double>(this->left, this->right);

	// Add each chromosome and interval to roulette wheel selection

	// TODO consider using a sorted map, when iterated through will go from largest to smallest
	// fitness (since the roulette wheel is most likely going to hit the large values and thus
	// save time in the look through)
	for (auto it = fitness.begin(); it != fitness.end(); ++it)
	{
		upper = lower + it->getResult();
		this->selection.insert({std::make_pair(lower, upper), it->getIndex()});
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

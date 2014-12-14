/**
 *  The MIT License (MIT)
 *
 * Copyright (c) 2014  Joseph Heron, Jonathan Gillett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

	// Right was not clearing and therefore the range continued to expand (always picked 0 for next).
	this->right = 0;

	// Is it really necessary for the left to even have a variable (it is always 0).
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

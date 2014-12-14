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

#ifndef ROULETTEWHEEL_HPP_
#define ROULETTEWHEEL_HPP_

#include <random>
#include <map>

#include "Selection.hpp"
#include "Result.hpp"

class RouletteWheel : public Selection
{
private:
    std::mt19937 engine;
    std::uniform_real_distribution<double> distribution;
    const double EPSILON = 1.0E-15;
    double left = 0.0;
    double right = 0.0 + EPSILON;
    std::map<std::pair<double, double>, unsigned int> selection;

public:
    /**
     * Default constructor for roulette wheel selection method, initializes
     * the random engine used for selection process.
     */
    RouletteWheel();

    virtual ~RouletteWheel();

    /**
     * Initialize the selection iterator with the fitness distribution of the
     * chromosomes which is used to generate the selection range of chromosomes
     * for the selection method.
     *
     * @param fitness The fitness distribution for the chromosomes to be used
     * for the roulette wheel selection.
     */
    virtual void init(std::vector<Result > &fitness);

    /**
     * The iterator method which uses roulette wheel selection to get the next
     * chromosome to be used for the genetic algorithm.
     *
     * @return The next chromosome selected.
     */
    virtual unsigned int next();
};

#endif /* ROULETTEWHEEL_HPP_ */

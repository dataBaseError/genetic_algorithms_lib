/*
 * Result.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: joseph
 */

#ifndef INC_RESULT_HPP_
#define INC_RESULT_HPP_

#include <vector>

class Result {
public:
	unsigned int index;
	double result;

	Result() {
		this->index = 0;
		this->result = 0;
	}

	Result(unsigned int index, double result) {
		this->index = index;
		this->result = result;
	}

};


#endif /* INC_RESULT_HPP_ */

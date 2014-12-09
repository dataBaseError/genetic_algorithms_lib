#ifndef INC_RESULT_HPP_
#define INC_RESULT_HPP_

#include <vector>

class Result {

	unsigned int index;
	double result;

public:
	

	Result();

	Result(unsigned int index, double result);

	unsigned int getIndex();

	void setIndex(unsigned int index);

	double getResult();

	void setResult(double result);

};


#endif /* INC_RESULT_HPP_ */

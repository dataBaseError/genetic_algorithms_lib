#ifndef INC_RESULT_HPP_
#define INC_RESULT_HPP_

//#include <vector>

class Result {

	unsigned int index;
	double result;

public:
	
	/*
	Result();

	Result(unsigned int index, double result);

	unsigned int getIndex();

	void setIndex(unsigned int index);

	double getResult();

	void setResult(double result);*/

	Result() {
		this->index = 0;
		this->result = 0;
	}

	Result(unsigned int index, double result) {
		this->index = index;
		this->result = result;
	}

	unsigned int getIndex() {
		return index;
	}

	void setIndex(unsigned int index) {
		this->index = index;
	}

	double getResult() {
		return result;
	}

	void setResult(double result) {
		this->result = result;
	}

};


#endif /* INC_RESULT_HPP_ */

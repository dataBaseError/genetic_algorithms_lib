#include "Result.hpp"


Result::Result() {
	this->index = 0;
	this->result = 0;
}

Result::Result(unsigned int index, double result) {
	this->index = index;
	this->result = result;
}

unsigned int Result::getIndex() {
	return index;
}

void Result::setIndex(unsigned int index) {
	this->index = index;
}

double Result::getResult() {
	return result;
}

void Result::setResult(double result) {
	this->result = result;
}
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

#ifndef SAFE_VECTOR_HPP_
#define SAFE_VECTOR_HPP_

#include <vector>

template <typename T >
class SafeVector {

private:
	std::vector<T > values;
	boost::mutex mtx_;

public:

	SafeVector()
	{
	}

	~SafeVector() {
	}

	bool empty() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		return values.empty();;
	}

	void clear() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		values.clear();
	}

	unsigned int size() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		return values.size();
	}
	
	void at(unsigned int index, T &result) {
		boost::unique_lock<boost::mutex> lock(mtx_);
		result = values[index];
	}

	T& at(unsigned int index) {
		boost::unique_lock<boost::mutex> lock(mtx_);
		return values[index];
	}

	// Not thread safe and therefore outside protection must be applied to ensure data integrity.
	// This is more meant to be used in the way to retrieve a local copy to safely use without 
	// the over kill of locking each access (lock once and then access as many as possible)
	void getAll(std::vector<T > &result) {

		result = values;
	}

	/**
	 * Simple push of a single value into the vector.
	 *
	 * @param The value to add to the vector.
	 *
	 */
	void push_back(T &val) {
		boost::unique_lock<boost::mutex> guard(mtx_);
		values.push_back(val);
	}

	/**
	 * Push a set of values into the vector.
	 * This is implemented to increase performance for a push that involves a
	 * large sum of entries which can be accomplished faster than fighting
	 * other threads for access to the mutex several times.
	 * NOTE entries cannot be a reference to the array of 'this' since insert
	 * is used and it does not allow for values to be inserted via iterator from
	 * the same vector.
	 * 
	 * @param entries The list of values to push into the vector.
	 */
	void push_back(std::vector<T > &entries) {
		boost::unique_lock<boost::mutex> guard(mtx_);

		values.insert(values.end(), entries.begin(), entries.end());
	}

	/** TODO use this function to copy the values into the vector.
	 * Copies the values from the entries vector into this vector starting at the index offset.
	 * This will over write any values stored within the range with the new values from the 
	 * entires vector.
	 * 
	 * Fails in the case of offset > this->size() - 1
	 *
	 * @param offset The index at which the values will start to be copied from.
	 * @param entries The elements of the vector will be set to the values of the given vector.
	 */
	void copy(unsigned int offset, std::vector<T > &entries) {
		boost::unique_lock<boost::mutex> lock(mtx_);
		std::copy(entries.begin(), entries.end(), this->values.begin()+offset);
	}

};


#endif /* SAFE_VECTOR_HPP_ */

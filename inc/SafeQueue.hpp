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

#ifndef SAFE_QUEUE_HPP_
#define SAFE_QUEUE_HPP_

#include <queue>
#include <boost/thread/condition_variable.hpp>

template <typename T >
class SafeQueue {

private:
	std::queue<T > values;
	boost::condition_variable m_cond;       // The condition to wait for
	boost::mutex mtx_;
	bool stop_waiting;

public:

	SafeQueue()
	{
		stop_waiting = false;
	}

	~SafeQueue() {
	}

	bool empty() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		bool e = values.empty();
		return e;
	}

	/**
	 * Retrieves the next value from the queue. The value is also removed.
	 * If the wait is to true the thread will sleep until the next value is
	 * available.
	 * 
	 * @param result The next value in the queue.
	 * @param wait Whether the pop will wait for a value if the queue is empty.
	 * Default is to wait for the a value if the queue is empty.
	 * 
	 * @return Whether the next value was retrieved or not.
	 *
	 */
	bool pop(T& result, bool wait=true) {

		boost::unique_lock<boost::mutex> lock(mtx_);

		while (values.size()==0 && !stop_waiting) {

			if(!wait) {
				// Queue is empty.
				return false;
			}

			m_cond.wait(lock);
		}

		// Exit the queue is finished
		if (stop_waiting) {
			return false;
		}

		result = values.front();
		values.pop();
		return true;
	}

	/**
	 * Retrieve the next amount of items from the queue. If the queue does not
	 * have enough values the thread will wait.
	 * 
	 * This method can be potentially dangerous since values may be left on
	 * the queue and never worked on if the the amount left on is less than the
	 * desired amount
	 * 
	 * @param amount The number of items to retrieve from the front the queue.
	 * @param results The output vector of the values retrieved.
	 * 
	 * @return Returns true if the number of values are within the results
	 * vector and false otherwise.
	 *
	 */
	bool pop(unsigned int amount, std::vector<T > &results) {
		boost::unique_lock<boost::mutex> lock(mtx_);

		// Initial design, can deadlock if no more values are added.
		while (values.size() < amount && !stop_waiting) {
			m_cond.wait(lock);
		}

		if(stop_waiting) {
			return false;
		}

		// Pop amount of values from the queue
		for(unsigned int i = 0; i < amount; i++) {

			results.push_back(values.front());
			values.pop();
		}
		return true;
	}

	/**
	 * Retrieves the next amount of items or less from the queue. If the queue
	 * is empty the thread will wait. Otherwise up to the amount of elements
	 * will be returned
	 * 
	 * @param amount The number of items to retrieve from the front the queue.
	 * @param results The output vector of the values retrieved.
	 * 
	 * @return Returns true if some number of values are within the results
	 * vector and false otherwise.
	 */
	bool popUpTO(unsigned int amount, std::vector<T > &results) {
		boost::unique_lock<boost::mutex> lock(mtx_);

		// Initial design, can deadlock if no more values are added.
		while (values.size()==0 && !stop_waiting) {
			m_cond.wait(lock);
		}

		if(stop_waiting) {
			return false;
		}

		// pop off upto amount of values from the queue
		for(unsigned int i = 0; i < amount || i < values.size(); i++) {

			results.push_back(values.front());
			values.pop();
		}
		return true;
	}

	/**
	 * Retrieve all the values within the queue. If no values are within the
	 * queue the thread will wait until there are some values within the queue.
	 *
	 * @param results All the values currently within the queue.
	 * 
	 * @return Returns true if values were successfully retrieved and false if
	 * the queue's finish method has been called.
	 */
	bool popAll(std::vector<T > &results, bool wait=true) {
		boost::unique_lock<boost::mutex> lock(mtx_);

		// Initial design, can deadlock if no more values are added.
		while (values.size()==0 && !stop_waiting) {

			if(!wait) {
				// Queue is empty.
				return false;
			}

			m_cond.wait(lock);
		}

		if(stop_waiting) {
			return false;
		}

		// pop off all the available values within the queue.
		while(values.size() != 0) {

			results.push_back(values.front());
			values.pop();
		}
		return true;
	}

	/**
	 * Simple push of a single value into the queue.
	 *
	 * @param The value to add to the queue.
	 *
	 */
	void push(T &val) {
		boost::unique_lock<boost::mutex> guard(mtx_);
		values.push(val);
		m_cond.notify_one();
	}

	/**
	 * Push a set of values into the queue.
	 * This is implemented to increase performance for a push that involves a
	 * large sum of entries which can be accomplished faster than fighting
	 * other threads for access to the mutex several times.
	 * 
	 * @param entries The list of values to push into the queue.
	 */
	void push(std::vector<T > &entries) {
		boost::unique_lock<boost::mutex> guard(mtx_);

		
		for(unsigned int i = 0; i < entries.size(); i++) {
			values.push(entries[i]);
			m_cond.notify_one();
		}
	}

	/**
	 * Notify all waiting threads to wake up and return false. This method
	 * essentially is a nice way of stopping the threads that are waiting for
	 * pop.
	 */
	void finish() {
		
		boost::unique_lock<boost::mutex> lock(mtx_);
		this->stop_waiting = true;
		m_cond.notify_all();
	}

};


#endif /* SAFE_QUEUE_HPP_ */

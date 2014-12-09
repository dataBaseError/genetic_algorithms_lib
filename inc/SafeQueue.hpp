/*
 * safe_queue.hpp
 *
 *  Created on: 2014-11-11
 *      Author: joseph
 */

#ifndef SAFE_QUEUE_HPP_
#define SAFE_QUEUE_HPP_

#include <queue>
#include <boost/thread/condition_variable.hpp>

//#include <pthread.h>
//#include <semaphore.h>

template <typename T >
class SafeQueue {

private:
	std::queue<T > values;
	boost::condition_variable m_cond;       // The condition to wait for
	boost::mutex mtx_;
	//pthread_mutex_t access_lock;
	//sem_t access_resource;
	bool stop_waiting;
	//pthread_cond_t empty_wait;

public:

	SafeQueue()// : sem_(0)
	{
		//pthread_mutex_init(&access_lock, NULL);
		//sem_init(&access_resource, 0, 0);
		stop_waiting = false;
		//pthread_cond_init(&empty_wait, 0);
		//pthread_cond_signal(&self_adapt_wait);
	}

	~SafeQueue() {
		//pthread_mutex_destroy(&access_lock);
		//sem_destroy(&access_resource);
		//pthread_cond_destroy(&empty_wait);
	}

	bool empty() {
		boost::unique_lock<boost::mutex> lock(mtx_);
		//pthread_mutex_lock(&access_lock);
		bool e = values.empty();
		//pthread_mutex_unlock(&access_lock);
		return e;
	}

	/* Didnt really need this
	typename std::queue<T >::size_type size() const {
		pthread_mutex_lock(&access_lock);
		auto s = values.size();
		pthread_mutex_unlock(&access_lock);
		return s;
	}*/

	/*
	T waitToPop() {
		boost::unique_lock<boost::mutex> guard(mtx_);
		//sem_wait(&access_resource);
		//if(stop_waiting) {
		//	return T ();
		//}
		//TODO add in exit conidtion
		return pop();
	}*/

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

		//T val = values.front();
		result = values.front();
		values.pop();
		//pthread_mutex_unlock(&access_lock);
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
	 * Simple push of a single value into the queue.
	 *
	 * @param The value to add to the queue.
	 *
	 */
	void push(T &val) {
		//pthread_mutex_lock(&access_lock);
		boost::unique_lock<boost::mutex> guard(mtx_);
		values.push(val);
		m_cond.notify_one();
		//sem_.post();
		//pthread_mutex_unlock(&access_lock);

		// Identify that more jobs are available
		//sem_post(&access_resource);
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

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

	// Condensed front and pop into a single method
	// Should return NULL when queue is empty and this method is called
	bool pop(T& result, bool wait=true) {

		//sem_.wait();
		//pthread_mutex_lock(&access_lock);
		boost::unique_lock<boost::mutex> lock(mtx_);

		while (values.size()==0 && !stop_waiting) {

			if(!wait) {
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

	void push(std::vector<T > &entries) {
		boost::unique_lock<boost::mutex> guard(mtx_);

		for(unsigned int i = 0; i < entries.size(); i++) {
			values.push(entries[i]);
			m_cond.notify_one();
		}
	}

	void finish() {
		
		boost::unique_lock<boost::mutex> lock(mtx_);
		this->stop_waiting = true;
		m_cond.notify_all();
	}

};


#endif /* SAFE_QUEUE_HPP_ */

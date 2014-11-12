/*
 * safe_queue.hpp
 *
 *  Created on: 2014-11-11
 *      Author: joseph
 */

#ifndef SAFE_QUEUE_HPP_
#define SAFE_QUEUE_HPP_

#include <queue>

#include <pthread.h>
#include <semaphore.h>

template <typename T >
class SafeQueue {

private:
	std::queue<T > values;
	pthread_mutex_t access_lock;
	sem_t access_resource;
	//pthread_cond_t empty_wait;

public:

	SafeQueue() {
		pthread_mutex_init(&access_lock, NULL);
		sem_init(&access_resource, 0, 0);
		//pthread_cond_init(&empty_wait, 0);
		//pthread_cond_signal(&self_adapt_wait);
	}

	~SafeQueue() {
		pthread_mutex_destroy(&access_lock);
		sem_destroy(&access_resource);
		//pthread_cond_destroy(&empty_wait);
	}

	bool empty() {
		pthread_mutex_lock(&access_lock);
		bool e = values.empty();
		pthread_mutex_unlock(&access_lock);
		return e;
	}

	/* Didnt really need this
	typename std::queue<T >::size_type size() const {
		pthread_mutex_lock(&access_lock);
		auto s = values.size();
		pthread_mutex_unlock(&access_lock);
		return s;
	}*/

	T waitToPop() {

		sem_wait(&access_resource);
		//TODO add in exit conidtion
		return pop();
	}

	// Condensed front and pop into a single method
	T pop() {
		pthread_mutex_lock(&access_lock);
		T val = values.front();
		values.pop();
		pthread_mutex_unlock(&access_lock);
		return val;
	}

	void push(T &val) {
		pthread_mutex_lock(&access_lock);
		values.push(val);
		pthread_mutex_unlock(&access_lock);

		// Identify that more jobs are available
		sem_post(&access_resource);
	}

};


#endif /* SAFE_QUEUE_HPP_ */

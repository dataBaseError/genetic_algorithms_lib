
#ifndef SAFE_VECTOR_HPP_
#define SAFE_VECTOR_HPP_

#include <vector>
//#include <boost/thread/condition_variable.hpp>

//#include <pthread.h>
//#include <semaphore.h>

template <typename T >
class SafeVector {

private:
	std::vector<T > values;
	//boost::condition_variable m_cond;       // The condition to wait for
	boost::mutex mtx_;
	//pthread_mutex_t access_lock;
	//sem_t access_resource;
	bool stop_waiting;
	//pthread_cond_t empty_wait;

public:

	SafeVector()// : sem_(0)
	{
		//pthread_mutex_init(&access_lock, NULL);
		//sem_init(&access_resource, 0, 0);
		stop_waiting = false;
		//pthread_cond_init(&empty_wait, 0);
		//pthread_cond_signal(&self_adapt_wait);
	}

	~SafeVector() {
		//pthread_mutex_destroy(&access_lock);
		//sem_destroy(&access_resource);
		//pthread_cond_destroy(&empty_wait);
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

		//boost::unique_lock<boost::mutex> lock(mtx_);
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

		/*for(unsigned int i = 0; i < entries.size(); i++) {
			values.push_back(entries[i]);
		}*/
	}

	/**
	 * Notify all waiting threads to wake up and return false. This method
	 * essentially is a nice way of stopping the threads that are waiting for
	 * pop.
	 */
	void finish() {
		
		boost::unique_lock<boost::mutex> lock(mtx_);
		this->stop_waiting = true;
		//m_cond.notify_all();
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

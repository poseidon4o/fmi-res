#pragma once

#include <cassert>

/// Queue container class, with linear memory and doubling resize when full
template <typename T>
struct Queue {
private:
	T *data = nullptr; ///< The storage for the items, allocated at some capacity and grows with factor 2
	int capacity = 0; ///< The allocated space for the @data
	int first = 0; ///< Index of the element that is at the front of the queue
	int last = 0; ///< Index of the first free spot where push will store the element
public:

	/// Initialize the queue with some capacity
	/// @param startCapacity - starting capacity, must be positive
	Queue(int startCapacity = 16) {
		assert(startCapacity > 0);
		capacity = startCapacity;
		data = new T[capacity];
	}

	~Queue() {
		delete[] data;
	}

	Queue(const Queue &other) {
		copy(other);
	}

	Queue &operator=(const Queue &other) {
		if (this == &other) {
			return *this;
		}

		delete[] data;
		copy(other);
		return *this;
	}

	/// Get the first inserted element
	T &front() {
		assert(!isEmpty());
		return data[first];
	}

	/// Get the first inserted element
	const T &front() const {
		assert(!isEmpty());
		return data[first];
	}

	/// Remove the first inserted element
	void pop() {
		assert(!isEmpty());
		first = next(first);
		assert(first >= 0 && first < capacity);
	}

	/// Insert an element at the end of the queue
	void push(const T& value) {
		if (shouldResize()) {
			resize();
		}
		data[last] = value;
		last = next(last);
		assert(last >= 0 && last < capacity);
	}

	/// Check if there are no elements in the queue
	bool isEmpty() const {
		return first == last;
	}

	int size() const {
		// in case the elements have wrapped around the end of the array, unwrap the index and calculate
		// the equal part will handle case when queue is empty
		const int movedLast = last >= first ? last : (last + capacity);
		return movedLast - first;
	}
private:

	/// Re-allocate the storage using temporary array
	/// Also rearranges the elements so that the are at the beginning of the array
	void resize() {
		T *newData = new T[capacity * 2];

		assert(size() == capacity - 1);
		for (int c = 0; c < capacity - 1; c++) {
			newData[c] = data[first];
			first = next(first);
		}

		first = 0;
		last = capacity - 1;
		capacity *= 2;
		delete[] data;
		data = newData;
	}

	/// Check if the data array has only 1 space and should be resized
	bool shouldResize() const {
		return (last + 1) % capacity == first;
	}

	/// Transform the passed 
	int next(int value) const {
		return (value + 1) % capacity;
	}

	/// Allocate and copy the contents of the passed object, assumes there is no allocated data
	void copy(const Queue &other) {
		data = new T[other.capacity];
		const int size = other.size();
		int copyIdx = other.first;
		for (int c = 0; c < size; c++) {
			data[c] = other.data[copyIdx];
			// use other.next since it will use correct capacity!
			copyIdx = other.next(copyIdx);
		}
		last = copyIdx;
		first = 0;
		capacity = other.capacity;
	}
};

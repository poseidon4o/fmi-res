#pragma once
#include <utility>
#include <random>
#include <cassert>

/// Join two heaps with the element that is parent for both
/// @param data - the array holding the heaps
/// @param size - the size of the array
/// @param parentIndex - the index of the element such that both it's left and right child are heaps
///                      the two heaps will be joined and will form new heap
void joinHeaps(int *data, int size, int parentIndex) {
	int current = parentIndex;
	while (current < size) {
		const int left = current * 2 + 1;
		const int right = current * 2 + 2;

		if (right > size) {
			break;
		}

		if (right == size) {
			if (data[current] < data[left]) {
				std::swap(data[current], data[left]);
			}
			break;
		}
		// else right < size -> left < size

		if (data[left] > data[right]) {
			if (data[left] > data[current]) {
				std::swap(data[current], data[left]);
				current = left;
			} else {
				break;
			}
		} else { // data[right] > data[current]
			if (data[right] > data[current]) {
				std::swap(data[current], data[right]);
				current = right;
			} else {
				break;
			}
		}
	}
}

/// Pop the top item in the heap, overwrites the top value
/// @param data - the heap
/// @param size - the number of items in the heap before the pop
void popHeap(int *data, int size) {
	data[0] = data[size - 1];
	joinHeaps(data, size, 0);
}

/// Make heap in-place using bottom-up strategy
/// @param data - the array
/// @param size - the number of items in the array
void makeHeapBottomUp(int *data, int size) {
	if (size == 1) {
		return;
	}

	const int reminder = size % 2;
	for (int c = size / 2 + reminder; c >= 0; c--) {
		joinHeaps(data, size, c);
	}
}

/// Push the last item in the heap, items in range [0, last) must already be heap
/// @param data - the data
/// @param last - the index of the element to push into the heap
void pushHeap(int *data, int last) {
	int current = last;

	while (current > 0) {
		const int parent = (current - 1) / 2;
		if (data[current] > data[parent]) {
			std::swap(data[current], data[parent]);
			current = parent;
		} else {
			break;
		}
	}
}

/// Make a heap in-place using top-down strategy
/// @param data - the data
/// @param size - the number of items in the array
void makeHeapTopDown(int *data, int size) {
	for (int c = 1; c < size; c++) {
		pushHeap(data, c);
	}
}

enum class HeapBuild {
	TopDown, BottomUp
};

/// Make heap using one of the two strategies
/// @param data - the data to build heap
/// @param size - the number of items in the array
/// @param build - the strategy to use
void makeHeap(int *data, int size, HeapBuild build = HeapBuild::BottomUp) {
	if (build == HeapBuild::BottomUp) {
		makeHeapBottomUp(data, size);
	} else if (build == HeapBuild::TopDown) {
		makeHeapTopDown(data, size);
	}
}

/// Use heap sort to sort the elements in ascending order
void heapSort(int *data, int size) {
	makeHeap(data, size);

	for (int c = size - 1; c > 0; c--) {
		const int value = data[0];
		popHeap(data, c + 1);
		data[c] = value;
	}
}


void test(std::mt19937 &generator, int size) {
	std::vector<int> mine;
	mine.reserve(size);
	for (int c = 0; c < size; c++) {
		mine.push_back(generator());
	}

	std::vector<int> copy = mine;
	std::sort(copy.begin(), copy.end());
	heapSort(mine.data(), mine.size());
	if (mine != copy) {
		__debugbreak();
		assert(false);
	}
}

void testHeapSort(int maxElements = 10000) {
	std::mt19937 generator(42);
	for (int c = 1; c < maxElements; c++) {
		for (int r = 0; r < 10; r++) {
			test(generator, c);
		}
	}
}

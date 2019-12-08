#include <vector>
#include <algorithm>
#include <random>

static const int maxCountRange = 1024 * 1024;

void countingSort(std::vector<int> &data) {
	if (data.size() < 2) {
		return;
	}
	const std::pair<std::vector<int>::iterator, std::vector<int>::iterator> minmax = std::minmax_element(data.begin(), data.end());
	const int range = *minmax.second - *minmax.first + 1;

	if (range > maxCountRange) {
		std::sort(data.begin(), data.end());
		return;
	}

	const int offset = *minmax.first;
	std::vector<int> counts(range, 0);
	for (int c = 0; c < data.size(); c++) {
		counts[data[c] - offset]++;
	}

	int out = 0;
	for (int c = 0; c < counts.size(); c++) {
		for (int r = 0; r < counts[c]; r++) {
			data[out++] = c + offset;
		}
	}
}

void test(std::mt19937 &generator, int size) {
	std::vector<int> mine;
	mine.reserve(size);

	std::uniform_int_distribution<int> dist(-1000, 1000);
	for (int c = 0; c < size; c++) {
		mine.push_back(dist(generator));
	}

	std::vector<int> copy = mine;
	std::sort(copy.begin(), copy.end());
	countingSort(mine);
	if (mine != copy) {
		__debugbreak();
		assert(false);
	}
}

void testCountingSort(int maxElements = 10000) {
	std::mt19937 generator(42);
	for (int c = 1; c < maxElements; c++) {
		for (int r = 0; r < 10; r++) {
			test(generator, c);
		}
	}
}
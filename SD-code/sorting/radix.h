#include <vector>
#include <type_traits>
#include <algorithm>
#include <random>
#include <cassert>

int powInt(int value, int power) {
	int product = 1;
	while (power--) {
		product *= value;
	}
	return product;
}

namespace Generic
{

template <typename T, typename RadixAccessor>
void radixStepGeneric(std::vector<T> &data, int iteration, std::vector<T> &output, std::vector<int> &counts, RadixAccessor getRadixIndex) {

	for (const T &item : data) {
		const int index = getRadixIndex(item, iteration);
		assert(index >= 0 && index < counts.size());
		++counts[index];
	}

	for (int c = 1; c < counts.size(); c++) {
		counts[c] += counts[c - 1];
	}

	for (int c = data.size() - 1; c >= 0; --c) {
		const int index = getRadixIndex(data[c], iteration);
		assert(index >= 0 && index < counts.size());
		output[counts[index] - 1] = data[c];
		--counts[index];
	}

	data.swap(output);

	memset(counts.data(), 0, counts.size() * sizeof(int));
}

template <typename T, typename RadixAccessor>
void radixSortGeneric(std::vector<T> &data, int radixSize, RadixAccessor getRadixIndex) {
	static_assert(std::is_convertible<decltype(getRadixIndex(data[0], 0)), int>::value, "Callback must return type convertible to integer");

	std::vector<T> output(data.size());
	std::vector<int> counts(radixSize, 0);

	for (int c = 0; c < radixSize; c++) {
		radixStepGeneric(data, c, output, counts, getRadixIndex);
	}
}

}

namespace Base10
{

void radixStep10(std::vector<int> &data, int exp, std::vector<int> &output) {
	int counts[10] = {0};

	for (int c = 0; c < data.size(); c++) {
		const int digit = (data[c] / exp) % 10;
		counts[digit]++;
	}

	for (int c = 1; c < 10; c++) {
		counts[c] += counts[c - 1];
	}

	for (int c = data.size() - 1; c >= 0; c--) {
		const int digit = (data[c] / exp) % 10;
		output[counts[digit] - 1] = data[c];
		counts[digit]--;
	}

	data.swap(output);
}

void radixSort10(std::vector<int> &data) {
	if (data.size() < 2) {
		return;
	}
	const int max = *std::max_element(data.begin(), data.end());
	const int iterations = log10(max) + 1;

	std::vector<int> output(data.size());
	for (int c = 0, exp = 1; c < iterations; c++, exp *= 10) {
		radixStep10(data, exp, output);
	}
}

}

namespace Base2InPlace {

void radixSort2InplaceRec(std::vector<int> &data, int from, int to, int digit) {
	if (digit == -1 || to - from + 1 < 2) {
		return;
	}
	const unsigned mask = 1 << digit;
	int left = from;
	int right = to;

	while (left < right) {
		const int bit = data[left] & mask;
		if (bit) {
			std::swap(data[left], data[right]);
			--right;
		} else {
			++left;
		}
	}

	assert(left == right);

	const int bit = (data[left] & mask) != 0;

	radixSort2InplaceRec(data, from, left - bit, digit - 1);
	radixSort2InplaceRec(data, left + !bit, to, digit - 1);
}

void radixSort2InplaceRec(std::vector<int> &data) {
	if (data.size() < 2) {
		return;
	}
	const int max = *std::max_element(data.begin(), data.end());
	const int msb = log2(max) + 1;

	radixSort2InplaceRec(data, 0, data.size() - 1, msb);
}

}

namespace BaseXInPlace {
// TODO
}

void test(std::mt19937 &generator, int size) {
	std::vector<int> mine;
	mine.reserve(size);

	std::uniform_int_distribution<int> dist(0, 10000);
	for (int c = 0; c < size; c++) {
		mine.push_back(dist(generator));
	}

	std::vector<int> copy = mine;
	std::sort(copy.begin(), copy.end());
	Base2InPlace::radixSort2InplaceRec(mine);
	if (mine != copy) {
		__debugbreak();
		assert(false);
	}
}

void testRadixSort(int maxElements = 10000) {
	std::mt19937 generator(42);
	for (int c = 1; c < maxElements; c++) {
		for (int r = 0; r < 10; r++) {
			test(generator, c);
		}
	}
}

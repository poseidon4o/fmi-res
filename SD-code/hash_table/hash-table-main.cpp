
#include "oo-hash-table.hpp"
#include "co-hash-table.hpp"

#include <cassert>
#include <ctime>

/// accept any container with typename templates
/// function will work correctly only if HashTable is actually a key-value associative container
template <template <typename ...> class HashTable>
void testTable() {

	// typedef these so its shorter and avoiding typename keyword spam
	typedef HashTable<int, int> IntHashT;
	typedef typename IntHashT::iterator ht_int_iterator;
	
	typedef HashTable<std::string, std::string> StringHashT;
	typedef typename StringHashT::iterator ht_string_iterator;

	typedef std::pair<const std::string, std::string> KeyValuePair;
	
	// some basic test
	const int mapSize = 10000;
	{
		puts("testing sequential ints");
		IntHashT ht;
		
		const int start = 0;
		for (int c = start; c < mapSize; c++) {
			if (c % 2) {
				ht.insert(c, c + 1);
				assert(ht.size() == c + 1);
				assert(ht[c] == c + 1);
			} else {
				ht[c] = c + 1;
				ht_int_iterator p = ht.find(c);
				assert(p != ht.end());
				assert(p->second == c + 1);
			}
		}

		for (int c = start; c < mapSize; c++) {
			ht_int_iterator p = ht.find(c);
			assert(p != ht.end());
			assert(p->second == c + 1);
		}

		ht_int_iterator it = ht.begin();
		while (it != ht.end()) {
			assert(it->first == it->second - 1);
			it = ht.erase(it);
		}

		assert(ht.size() == 0);
	}

	srand(time(nullptr));

	{
		std::unordered_map<std::string, std::string> stdMap;
		StringHashT ht;
		puts("generating collision string maps");
		const int uniqueElements = mapSize / 10;
		for (int c = 0; c < mapSize; c++) {
			char key[128], value[128];
			snprintf(key, sizeof(key), "key-%d", rand() % uniqueElements);
			snprintf(value, sizeof(value), "val-%d-%d", rand(), rand());
			stdMap[key] = value;
			ht[key] = value;
		}

		puts("checking string maps");
		assert(ht.size() == stdMap.size());

		for (const KeyValuePair & stdItem : stdMap) {
			ht_string_iterator item = ht.find(stdItem.first);
			assert(item != ht.end());
			assert(item->second == stdItem.second);
			assert(item->first == stdItem.first);
		}
		puts("All items are accounted for");

		for (const KeyValuePair & htItem : ht) {
			std::unordered_map<std::string, std::string>::iterator item = stdMap.find(htItem.first);
			assert(item != stdMap.end());
			assert(item->second == htItem.second);
			assert(item->first == htItem.first);
		}

		puts("There are no extra items");

		ht_string_iterator it = ht.begin();
		while (it != ht.end()) {
			std::unordered_map<std::string, std::string>::iterator item = stdMap.find(it->first);
			assert(item != stdMap.end());
			stdMap.erase(item);
			it = ht.erase(it);
		}

		assert(stdMap.size() == ht.size());
	}
}

template <typename K, typename Hash = std::hash<K>>
struct QuadraticProber
{
	Hash hasher;
	size_t lastHash;
	size_t operator()(const K& key, int iteration, int size) {
		if (iteration == 0) {
			lastHash = hasher(key);
			return lastHash;
		}
		return lastHash + iteration * iteration;
	}
};

template <typename K, typename T>
using QuadraticHashTable = OOHashTable<K, T, QuadraticProber<K>>;


unsigned int MurmurHash2(const void * key, int len, unsigned int seed) {
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int h = seed ^ len;
	const unsigned char * data = (const unsigned char *) key;
	while (len >= 4) {
		unsigned int k = *(unsigned int *) data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}
	switch (len) {
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	return h;
}

template <typename K>
struct MurMurHasher
{
	size_t operator()(const K& key) const {
		return MurmurHash2(&key, sizeof(key), 42);
	}
};

template <>
struct MurMurHasher<std::string> {
	size_t operator()(const std::string& key) const {
		return MurmurHash2(key.data(), key.length(), 42);
	}
};

template <typename K, typename SecondHash, typename Hash = std::hash<K>>
struct DoubleHash {
	size_t lastHash, secondHash;
	SecondHash secondHasher;
	Hash hasher;
	size_t operator()(const K& key, int iteration, int size) {
		if (iteration == 0) {
			lastHash = hasher(key) % size;
			return lastHash;
		}
		if (iteration == 1) {
			//secondHash = (7 - (lastHash % 7));
			secondHash = secondHasher(key);
			secondHash += secondHash % size == 0 ? 1 : 0;
		}

		return lastHash + iteration * secondHash;
	}
};

template <typename K, typename T, typename Hash = std::hash<K>>
using DoubleHashTable = OOHashTable<K, T, DoubleHash<K, MurMurHasher<K>>>;



int main()
{
	//puts("- closed addressing hash table");
	//testTable<COHashTable>();
	//puts("- done");

	//puts("- open addressing hash table");
	//testTable<OOHashTable>();
	//puts("- done");

	//puts("- open addressing hash table with quadratic probing");
	//testTable<QuadraticHashTable>();
	//puts("- done");

	puts("- open addressing hash table with quadratic probing");
	testTable<DoubleHashTable>();
	puts("- done");

	puts("press enter to exit");
	getchar();
}

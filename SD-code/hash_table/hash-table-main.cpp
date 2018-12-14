
#include "oo-hash-table.hpp"
#include "co-hash-table.hpp"

/// accept any container with typename templates
/// function will work correctly only if HashTable is actually a key-value associative container
template <template <typename ...> class HashTable>
void testTable() {

	// typedef these so its shorter and avoiding typename keyword spam
	typedef HashTable<int, int> IntHashT;
	typedef typename IntHashT::iterator ht_int_iterator;
	
	typedef HashTable<std::string, std::string> StringHashT;
	typedef typename StringHashT::iterator ht_string_iterator;
	
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
		puts("generating string maps");
		for (int c = 0; c < mapSize; c++) {
			char key[128], value[128];
			snprintf(key, sizeof(key), "key-%d-%d", rand(), rand());
			snprintf(value, sizeof(value), "val-%d-%d", rand(), rand());
			stdMap[key] = value;
			ht[key] = value;
		}

		puts("checking string maps");
		assert(ht.size() == stdMap.size());

		for (const auto & stdItem : stdMap) {
			ht_string_iterator item = ht.find(stdItem.first);
			assert(item != ht.end());
			assert(item->second == stdItem.second);
			assert(item->first == stdItem.first);
		}
		puts("All items are accounted for");

		for (const auto & htItem : ht) {
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

		for (const auto & stdItem : stdMap) {
			ht_string_iterator item = ht.find(stdItem.first);
			assert(item != ht.end());
			assert(item->second == stdItem.second);
			assert(item->first == stdItem.first);
		}
		puts("All items are accounted for");

		for (const auto & htItem : ht) {
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

int main()
{
	puts("- closed addressing hash table");
	testTable<COHashTable>();
	puts("- done");

	puts("- open addressing hash table");
	testTable<OOHashTable>();
	puts("- done");

	puts("press enter to exit");
	getchar();
}

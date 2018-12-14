#pragma once

#include <vector>
#include <cassert>
#include <unordered_map>
#include <ctime>


/// Closed addressing hash table, templated by key, value and hash of key
template <typename K, typename T, typename Hash = std::hash<K>>
class COHashTable {
public:
	typedef std::pair<K, T> pair_type;
	typedef T value_type;

	typedef value_type & reference;
	typedef const value_type & const_reference;
	typedef value_type * pointer;

	typedef pair_type & pair_reference;
	typedef pair_type * pair_pointer;
	typedef const pair_type & pair_const_reference;

private:
	typedef std::vector<pair_type> bucket_type;
	typedef std::vector<bucket_type> table_type;

	table_type table; /// The table data
	int count; ///< Number of elements inserted in the table
	Hash hasher; ///< Hasher object

	/// Get the bucket index for a given key
	int index(const K &key) const {
		return hasher(key) % table.size();
	}

	/// Get iterator to the bucket for a given key, always valid iterator
	typename table_type::iterator getBucket(const K &key) {
		return table.begin() + index(key);
	}

	/// Check if table has reached maxLoadFactor
	bool shouldResize() {
		const float load = float(count) / table.size();
		return load > 0.7;
	}

	/// Allocate more space and re-hash the table
	void resize() {
		table_type newTable(table.size() * 2 + 1);
		// swap the tables now so we can use the private utility methods (index, getBucket)
		table.swap(newTable);

		for (auto & bucket : newTable) {
			for (auto & el : bucket) {
				// directly insert to avoid checking for duplicating keys
				// since this is called only on valid elements, duplicate keys will not be present
				getBucket(el.first)->push_back(std::make_pair(el.first, el.second));
			}
			// clear this source bucket since all elements from it are transferred to the new table
			// this will allow the resize() method to only require O(n) + O(largestBucket) memory
			bucket.clear();
		}
	}

public:
	COHashTable(Hash hasher = Hash())
		: table(32)
		, count(0)
		, hasher(hasher) {
	}

	void clear() {
		table = table_type(32);
		count = 0;
	}

	class iterator {
		// friend the container so it can access the private constructors
		friend class COHashTable;
		
		table_type *table; ///< Pointer so the iterators can be easily copy-able
		typename table_type::iterator bucket; ///< Iterator to the current bucket
		typename bucket_type::iterator element; ///< Iterator to the current element

		/// Creates the begin iterator for the given table
		iterator(table_type &tableRef): table(&tableRef) {
			// bucket will never be table->end(), because the table will always have some buckets
			bucket = table->begin();
			element = bucket->begin();
			findNextValid();
		}

		/// Creates iterator to a specific element
		iterator(table_type &table, typename table_type::iterator bucket, typename  bucket_type::iterator element)
			: table(&table)
			, bucket(bucket)
			, element(element)
		{}

	public:
		/// Get pair, but cast it to const key, so caller can't edit the key
		std::pair<const K, T> & operator*() const {
			// cast from pair_type -> to const_key_pair_type
			// we can safely reinterpret this as they are binary the same 
			return reinterpret_cast<std::pair<const K, T> &>(*element);
		}

		std::pair<const K, T> * operator->() const {
			// re-use the operator* to avoid writing the cast
			return &(operator*());
		}

		iterator operator++(int) {
			iterator copy(*this);
			advance();
			return copy;
		}

		iterator& operator++() {
			advance();
			return *this;
		}

		bool operator==(const iterator &other) const {
			// Since end() iterators are all the same - just check for full equality
			return table == other.table && bucket == other.bucket && element == other.element;
		}

		bool operator!=(const iterator &other) const {
			return !(*this == other);
		}

	private:
		/// Assuming iterator points to valid element, advance it to the next valid
		/// or to the end() iterator position
		void advance() {
			++element;
			findNextValid();
		}

		/// If element is end iterator of current bucket, find the next valid bucket or
		/// get to end() if there are none
		void findNextValid() {
			// already valid element
			if (element != bucket->end()) {
				return;
			}

			// find first bucket with elements
			do {
				++bucket;
			} while (bucket != table->end() && bucket->empty());

			// if bucket reached table->end(), make this end() iterator
			if (bucket != table->end()) {
				element = bucket->begin();
			} else {
				element = table->back().end(); // end iterator
			}
		}
	};

	/// Iterator to first element or end() if table is empty
	iterator begin() {
		return iterator(table);
	}

	/// End iterator, can only be used for equality check
	iterator end() {
		return iterator(table, table.end(), table.back().end());
	}

	/// Find an element by its key, returns iterator to the element or end() if not found
	iterator find(const K &key) {
		auto bucket = getBucket(key);
		for (auto elIter = bucket->begin(); elIter != bucket->end(); ++elIter) {
			if (elIter->first == key) {
				return iterator(table, bucket, elIter);
			}
		}
		return end();
	}

	/// Insert key-value pair, if key is already present in the table, overwrites the value
	iterator insert(const K &key, const T &value) {
		// do check before inserting as it can invalidate iterators!
		if (shouldResize()) {
			resize();
		}

		auto bucket = getBucket(key);
		for (auto elIter = bucket->begin(); elIter != bucket->end(); ++elIter) {
			// if key matches, return iterator to it
			if (elIter->first == key) {
				// overwrite the value
				elIter->second = value;
				return iterator(table, bucket, elIter);
			}
		}

		++count;
		typename bucket_type::iterator element = bucket->insert(bucket->end(), std::make_pair(key, value));
		return iterator(table, bucket, element);
	}

	/// Get value reference to an element with given key,
	/// if key is not in the table, default construct the value and insert it
	reference operator[](const K &key) {
		auto it = find(key);
		if (it == end()) {
			return insert(key, T())->second;
		} else {
			return it->second;
		}
	}

	/// Erase the element pointed by the iterator and return iterator to the next element
	iterator erase(iterator it) {
		// do this check so the table can support this: table.erase(table.find(someKey));
		if (it == end()) {
			return it;
		}

		--count;
		it.element = it.bucket->erase(it.element);
		it.findNextValid();
		return it;
	}

	/// Erase item by key, returns iterator to next valid element
	/// If key is not in the map, return end() iterator
	iterator erase(const K &key) {
		return erase(find(key));
	}

	/// Get the number of key-value pairs in the table
	int size() const {
		return count;
	}
};

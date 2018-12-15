#pragma once

#include <vector>
#include <unordered_map>
#include <cassert>

template <typename K, typename Hash = std::hash<K>>
struct OOHashProbeLinear {
	Hash hasher;
	size_t operator() (const K &key, int iteration, int size) const {
		return hasher(key) + iteration;
	}
};


/// Open addressing hash table, templated by key, value, hash functor and function for probing on collision
/// Also the IndexProbe must not have fixed point
template <typename K, typename T, typename Hash = OOHashProbeLinear<K>>
class OOHashTable
{
public:
	typedef std::pair<K, T> pair_type;

	typedef K key_type;
	typedef T value_type;

	typedef value_type & reference;
private:
	
	struct Bucket {
		pair_type data; ///< Key value pair
		bool empty = true; ///< true for empty or deleted buckets
		bool deleted = false; ///< true when element was removed - can be re-used in insert
	};

	typedef std::vector<Bucket> table_t;
	typedef typename table_t::iterator bucket_iterator;

	table_t table; ///< The table data
	int count; ///< Actual number of elements
	Hash hasher; ///< The hash functor

	/// Check if the table needs to be resized
	bool needsResize() const {
		const float factor = float(count) / table.size();
		return factor >= 0.7;
	}

	/// Resize and re-hash the table
	void resize() {
		table_t newTable(table.size() * 2 + 1);
		// swap with member so we can re-use insert
		newTable.swap(table);

		// since insert is re-used, it will increment count for each element
		// thus zero it here so the end count is correct
		count = 0;
		for (Bucket & el : newTable) {
			if (!el.empty && !el.deleted) {
				// re-use insert to avoid duplicating the collision logic
				insert(el.data.first, el.data.second);
			}
		}
	}

	/// Find the correct bucket for a given key
	/// If checkDeleted is set, then finds non deleted buckets
	/// If nextIndex is guaranteed to walk every index then this will always terminate eventually
	bucket_iterator findBucket(const K &key, bool checkDeleted) {
		int iter = 0;
		int idx = hasher(key, iter++, table.size()) % table.size();

		// do endless loop and move checks inside to improve readability
		while (true) {
			// can stop on free bucket
			if (table[idx].empty && (!checkDeleted || checkDeleted && !table[idx].deleted)) {
				break;
			}
			// key could match by might be deleted
			if (table[idx].data.first == key && (!checkDeleted || checkDeleted && !table[idx].deleted)) {
				break;
			}
			const int before = idx;
			idx = hasher(key, iter++, table.size()) % table.size();
			assert(before != idx);
		}

		return table.begin() + idx;
	}
	
public:
	OOHashTable(Hash hash = Hash())
		: table(41)
		, count(0)
		, hasher(hash) {}

	/// Iterator over the key-value pairs in the table
	class iterator {
		friend class OOHashTable;
		table_t *table; ///< Pointer to the table, not reference so the class can have operator=
		bucket_iterator element; ///< Iterator to the element

		/// Construct from some table and iterator and move to the first valid element or the end() iterator
		iterator(table_t &table, bucket_iterator el)
			: table(&table)
			, element(el)
		{
			validateIterator();
		}

		/// If the current iterator points to empty bucket move it forward until end() or valid bucket
		void validateIterator() {
			while (element != table->end() && element->empty) {
				++element;
			}
		}
	public:
		/// Pair with const first element so key can be immutable to the user of the iterator
		typedef std::pair<const K, T> const_pair;

		/// Get reference to the key value pair
		const_pair & operator*() {
			// must return const key pair because caller could change they key and this will
			// invalidate the HashTable invariants, it is safe to reinterpret cast it to const key since both share same binary layout
			return reinterpret_cast<const_pair &>(*element);
		}

		/// Pointer to the key-value pair
		const_pair * operator->() {
			// re-use the operator* to avoid reinterpret_cast here
			return &(operator*());
		}

		/// Prefix increment
		iterator& operator++() {
			++element;
			validateIterator();
			return *this;
		}

		/// Postfix increment
		iterator operator++(int) {
			iterator copy(*this);
			++element;
			validateIterator();
			return copy();
		}

		/// Equality check, iterators must be from the same table
		bool operator==(const iterator &other) const {
			return table == other.table && element == other.element;
		}

		/// Opposite of operator==
		bool operator!=(const iterator &other) const{
			return !(*this == other);
		}
	};

	/// First valid key-value pair or end() if table is empty
	iterator begin() {
		return iterator(table, table.begin());
	}

	/// End iterator can be used only for equality checks with
	iterator end() {
		return iterator(table, table.end());
	}

	iterator insert(const K &key, const T&value) {
		if (needsResize()) {
			resize();
		}

		bucket_iterator bucket = findBucket(key, false); // ignore deleted flag when inserting
		assert(!bucket->empty || bucket->empty ^ bucket->deleted);
		if (bucket->empty) {
			++count;
		}
		bucket->deleted = false;
		bucket->empty = false;
		bucket->data = std::make_pair(key, value);

		return iterator(table, bucket);
	}

	/// Erase an item and return iterator to the next valid item or end()
	iterator erase(iterator it) {
		// check for end erase(find(somKey)) works as expected
		if (it == end()) {
			return it;
		}
		assert(!it.element->empty || it.element->empty ^ it.element->deleted);
		if (!it.element->empty) {
			--count;
			it.element->deleted = it.element->empty = true;
		}

		it.validateIterator();
		return it;
	}

	/// Erase element by key and return iterator to next element in map or end()
	iterator erase(const K &key) {
		return erase(find(key));
	}

	/// Get iterator for a given key or end() if key is not inserted
	iterator find(const K &key) {
		bucket_iterator bucket = findBucket(key, true); // must not be deleted
		assert(!bucket->empty || bucket->empty ^ bucket->deleted);
		if (bucket->empty) {
			return end();
		}
		return iterator(table, bucket);
	}

	/// Get reference to a based on a key, if not present insert default constructed value
	T & operator[](const K&key) {
		if (float(count) / table.size() > 0.7) {
			resize();
		}

		iterator element = find(key);
		if (element != end()) {
			assert(!element.element->empty || element.element->empty ^ element.element->deleted);
			return element->second;
		}
		
		return insert(key, T())->second;
	}

	/// Get the number of key-value pairs in the map
	int size() const {
		return count;
	}
};

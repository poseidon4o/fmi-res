#pragma once

#include <cassert>

template <typename T>
class List {
	struct Node {
		Node *prev = nullptr;
		Node *next = nullptr;
		T data;
		Node(const T &data)
			: data(data)
		{}

		Node() {}
	};

	// TODO: have the same logic with dummy element but avoid wasting space for one additional T()
	Node dummy; // sacrifice space for one T to avoid complexity
	int size = 0;

	void insertAfterNode(Node *node, const T &value) {
		Node *newNode = new Node;
		newNode->data = value;

		// Attach node to new neighbours
		newNode->next = node->next;
		newNode->prev = node;

		// Attach neighbours to node
		node->next->prev = newNode;
		node->next = newNode;
		++size;
	}

	void removeNode(Node *node) {
		assert(node != &dummy);
		assert(size > 0);
		--size;
		// Attach prev and next to eachother
		node->next->prev = node->prev;
		node->prev->next = node->next;
		delete node;
	}

	void copy(const List<T> &other) {
		assert(isEmpty());
		for (const_iterator it = other.begin(); it != other.end(); ++it) {
			pushBack(*it);
		}
	}
public:
	// TODO: add support for reverse iteration
	//       list::rbegin(), list::rend(), list::crbegin()
	class iterator {
		friend class List;
		Node *current = nullptr;

		/// Private so only List has access to this ctor
		explicit iterator(Node *node)
			: current(node)
		{}
	public:
		iterator() {}

		T &operator*() {
			return current->data;
		}

		T *operator->() {
			return &current->data;
		}

		/// Allow for const iterator to have access to the data
		const T &operator*() const {
			return current->data;
		}

		const T *operator->() const {
			return &current->data;
		}

		bool operator==(const iterator &other) const {
			return current == other.current;
		}

		bool operator!=(const iterator &other) const {
			return !(*this == other);
		}

		iterator &operator++(int) {
			current = current->next;
			return *this;
		}

		iterator operator++() {
			iterator copy(*this);
			current = current->next;
			return copy;
		}

		iterator &operator--(int) {
			current = current->prev;
			return *this;
		}

		iterator operator--() {
			iterator copy(*this);
			current = current->prev;
			return copy;
		}
	};

	/// Different class to allow creation from const methods from the list like (cbegin() const/begin() const/end() const)
	class const_iterator {
		friend class List;
		const Node *current = nullptr;

		/// Private so only List has access to this ctor
		explicit const_iterator(const Node *node)
			: current(node)
		{}
	public:
		const_iterator() {}

		/// Read access to the contained data
		const T &operator*() {
			return current->data;
		}

		const T *operator->() {
			return &current->data;
		}

		/// Allow access from const const_iterator
		const T &operator*() const {
			return current->data;
		}

		const T *operator->() const {
			return &current->data;
		}

		/// Compares for iteration termination
		bool operator==(const const_iterator &other) const {
			return current == other.current;
		}

		bool operator!=(const const_iterator &other) const {
			return !(*this == other);
		}

		/// Allow cross-compare with non const iterator to allow const_iterator() != list.end()
		bool operator==(const iterator &other) const {
			return current == other.current;
		}

		bool operator!=(const iterator &other) const {
			return !(*this == other);
		}

		const_iterator &operator++(int) {
			current = current->next;
			return *this;
		}

		const_iterator operator++() {
			const_iterator copy(*this);
			current = current->next;
			return copy;
		}

		const_iterator &operator--(int) {
			current = current->prev;
			return *this;
		}

		const_iterator operator--() {
			const_iterator copy(*this);
			current = current->prev;
			return copy;
		}
	};

	List() {
		dummy.prev = dummy.next = &dummy;
	}

	~List() {
		clear();
	}

	List(const List<T> &other) {
		dummy.prev = dummy.next = &dummy;
		copy(other);
	}

	List<T> &operator=(const List<T> &other) {
		if (this == &other) {
			return *this;
		}
		clear();
		copy(other);
		return *this;
	}

	/// Remove all elements from the list
	void clear() {
		while (!isEmpty()) {
			popBack();
		}
	}

	/// Iterator accessors
	const_iterator begin() const {
		return const_iterator(dummy.next);
	}

	const_iterator cbegin() const {
		return const_iterator(dummy.next);
	}

	iterator begin() {
		return iterator(dummy.next);
	}

	iterator end() {
		return iterator(&dummy);
	}

	const_iterator end() const {
		return const_iterator(&dummy);
	}

	/// Mutators for both ends
	void popBack() {
		removeNode(dummy.prev);
	}

	void popFront() {
		removeNode(dummy.next);
	}

	void pushFront(const T &value) {
		insertAfterNode(&dummy, value);
	}

	void pushBack(const T &value) {
		insertAfterNode(dummy.prev, value);
	}

	/// Mutators with given position (iterator)
	void insertAfter(const iterator &it, const T &value) {
		insertAfterNode(it.current, value);
	}

	void insertBefore(const iterator &it, const T &value) {
		insertAfterNode(it.current->prev, value);
	}

	void remove(const iterator &it) {
		removeNode(it.current);
	}

	/// Steal all elements from @other and insert them at @where
	void splice(List<T> &other, iterator where) {
		if (other.isEmpty()) {
			return;
		}

		Node *destination = where.current;
		Node *source = &other.dummy;

		// Attach elements from @other to our elements
		source->next->prev = destination;
		source->prev->next = destination->next;

		// Attach our elements to other's elements
		destination->next->prev = source->prev;
		destination->next = source->next;

		// Detach other's dummy from it's elements
		size += other.size;
		source->next = source->prev = source;
		other.size = 0;

		assert(other.isEmpty());
	}

	/// Steal all elements from @other and insert them after the last element
	void splice(List<T> &other) {
		iterator it = end();
		splice(other, --it);
	}

	/// Size checks
	bool isEmpty() const {
		const bool empty = dummy.prev == &dummy;
		assert(empty == (size == 0));
		return empty;
	}

	int getSize() const {
		return size;
	}
};
template <typename T>
class Tree {
	struct Node {
		Node *left = nullptr;
		Node *right = nullptr;
		T value;
	};

	Node *root;

	Node *& find(Node *& n, const T &value) {
		if(!n || n->value == value) {
			return n;
		}

		if (value > n->value) {
			return find(n->right, value);
		} else {
			return find(n->left, value);
		}
	}

public:
	Tree(): root(nullptr) {}

	void insert(const T &value) {
		Node *& n = find(root, value);
		n->value = value;
	}

	bool find(const T&value) {
		return find(root, value) != nullptr;
	}

	bool remove(const T&value) {
		Node *& n = find(root, value);
		if (!n) {
			return false;
		}

		if (!n->left && !n->right) {
			delete n;
			n = nullptr;
		} else if (n->left) {
			Node *del = n;
			n = n->left;
			delete del;
		} else if (n->right) {
			Node *del = n;
			n = n->right;
			delete del;
		} else {
			
		}
	}
};

int main() {
	int *p;
	alloc(p, 100);
}
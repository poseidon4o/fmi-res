#include <algorithm>
#include <fstream>

struct Node {
	Node *left, *mid, *right;
	char val;
};

int depth(Node *root) {
	if (!root) {
		return 0;
	}
	return 1 + std::max(
		depth(root->left),
		depth(root->right),
		depth(root->mid)
	);
}

int findAtDepth(Node *root, int depth) {
	if (!root) {
		return 0;
	}
	if (depth == 0) {
		return 1;
	}
	return findAtDepth(root->left, depth - 1)
		 + findAtDepth(root->mid, depth - 1)
		 + findAtDepth(root->right, depth - 1);
}

void concatLast(Node *root, int depth, char *&str) {
	if (!root) {
		return;
	}
	if (depth == 0) {
		*str = root->val;
		++str;
		return;
	}

	concatLast(root->left, depth - 1, str);
	concatLast(root->mid, depth - 1, str);
	concatLast(root->right, depth - 1, str);
}

char * readLast(Node *root) {
	int maxDepth = depth(root);
	int len = findAtDepth(root, maxDepth);
	char * str = new char[len + 1];
	char *first = str;
	concatLast(root, maxDepth, str);
	*str = 0;
	return first;
}

void serialize(std::ofstream &file, Node *root) {
	if (!root) {
		file << '*';
		return;
	}

	file << '(' << root->val << ' ';
	serialize(file, root->left);
	file << ' ';
	serialize(file, root->mid);
	file << ' ';
	serialize(file, root->right);
	file << ')';
}

void serialize(Node *root, const char *path) {
	std::ofstream file(path);
	serialize(file, root);
}
// 15.07.2014, problem 7
#include <cmath>
struct Node
{
	Node *next;
	int value;
};

Node *preceeds(Node *list, int first, int second) {
	if (!list) {
		return nullptr;
	}

	// handles case 2 element list where list[0] == second and list[1] == first
	if (list == list->next->next) {
		if (list->value == second && list->next->value == first) {
			return list->next;
		}
	}

	Node *iter = list;
	while (iter->next != list) {
		if (iter->value == first) {
			if (iter->next->value == second) {
				return iter;
			}
		}
		iter = iter->next;
	}

	return nullptr;
}

void merge(Node *a, Node *b) {
	if (!a || !b) {
		return;
	}

	if (a == a->next || b == b->next) {
		return;
	}

	Node *secondListMaxSum = nullptr;
	Node *firstListMaxSum = nullptr;

	Node *iter = a;
	bool shouldBreak = false;
	while (!shouldBreak) {
		if (iter->next == a) {
			shouldBreak = true;
		}
		int first = iter->value;
		int second = iter->next->value;
		Node *current = preceeds(b, second, first);
		if (current) {
			if (!secondListMaxSum) {
				firstListMaxSum = iter;
				secondListMaxSum = current;
			} else {
				int maxSum = secondListMaxSum->value + secondListMaxSum->next->value;
				if (first + second > maxSum) {
					secondListMaxSum = current;
					firstListMaxSum = iter;
				}
			}
		}
		iter = iter->next;
	}

	if (!secondListMaxSum) {
		return;
	}

	Node *secondDelete = secondListMaxSum->next;
	Node *firstDelete = firstListMaxSum->next;

	firstListMaxSum->next = secondDelete->next;
	secondListMaxSum->next = firstDelete->next;

	delete secondDelete;
	delete firstDelete;
}

int length(Node *n) {
	Node *iter = n;
	int len = 0;
	while (iter->next != n) {
		++len;
	}
	return len + 1;
}

Node *findBestPair(Node *list, int second, int totalLen, int &len) {
	Node *bestSplit = nullptr;
	int bestSplitDiff = totalLen;
	
	Node *iter = list;
	for (int c = 0; c < totalLen; c++) {
		if (iter->value == second) {
			int currentSplit = std::abs(totalLen - c - c);
			if (!bestSplit) {
				bestSplit = iter;
				bestSplitDiff = currentSplit;
			} else if (bestSplitDiff > currentSplit) {
				bestSplit = iter;
				bestSplitDiff = currentSplit;
			}
		}

		iter = iter->next;
	}

	len = bestSplitDiff;
	return bestSplit;
}

void split(Node *list, int first, int second, Node *& a, Node *& b) {
	a = b = nullptr;
	if (!list || list == list->next || list->next->next == list) {
		return;
	}

	int totalLen = length(list);
	// best pair of nodes based on difference between then
	Node *bestFirst = nullptr;
	Node *bestSecond = nullptr;
	int bestDiff = totalLen;

	Node *iter = list;
	for (int c = 0; c < totalLen; c++) {
		if (iter->value == first) {
			int currentSplitDiff = 0;
			Node *currentSplit = findBestPair(iter, second, totalLen, currentSplitDiff);
			if (!currentSplit) {
				return;
			}
			if (!bestFirst) {
				bestFirst = iter;
				bestSecond = currentSplit;
				bestDiff = currentSplitDiff;
			} else if (bestDiff > currentSplitDiff) {
				bestFirst = iter;
				bestSecond = currentSplit;
				bestDiff = currentSplitDiff;
			}
		}

		iter = iter->next;
	}

	Node *firstNew = new Node;
	Node *secondNew = new Node;

	firstNew->value = bestSecond->value;
	secondNew->value = bestFirst->value;

	firstNew->next = bestSecond->next;
	secondNew->next = bestFirst->next;

	bestSecond->next = secondNew;
	bestFirst->next = firstNew;

	a = firstNew;
	b = secondNew;
}


int main() {
	
	return 0;
}
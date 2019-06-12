struct Node {
	Node *prev, *next;
	int val;
};

bool isSorted(Node *list) {
	if (!list) {
		return true;
	}
	while (list->next) {
		if (list->next->val < list->val) {
			return false;
		}
	}

	return true;
}

void insertBefore(Node *list, Node *n) {
	list->prev->next = n;
	n->prev = list->prev;

	n->next = list;
	list->prev = n;
}

void mergeToLeft(Node *list, Node *toMerge) {
	while (list) {
		if (list->val > toMerge->val) {
			while (toMerge && list->val > toMerge->val) {
				Node *next = toMerge->next;
				insertBefore(list, toMerge);
				toMerge = next;
			}
		}
		list = list->next;
	}
}

struct ListNode {
	ListNode *next, *prev;
	Node *val;
};

ListNode * removeCurrent(ListNode *n) {
	ListNode *next = n->next;
	if (next) {
		next->prev = n->prev;
	}
	if (n->prev) {
		n->prev->next = next;
	}
	delete n;
	return next;
}

Node *mergeLists(ListNode *listList) {
	Node *merged = nullptr;
	while (listList) {
		if (isSorted(listList->val)) {
			if (!merged) {
				merged = listList->val;
			} else {
				mergeToLeft(merged, listList->val);
			}
			listList = removeCurrent(listList);
		} else {
			listList = listList->next;
		}
	}
}

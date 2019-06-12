struct Node {
	Node *prev, *next;
	int value;
};

struct List {
	Node fist;
	Node last;
};

int findJoin(Node *firstA, Node *lastB, Node *& aMid, Node *& bMid) {
	aMid = bMid = nullptr;

	for (int c = 0; firstA && lastB; c++) {
		if (firstA->value == lastB->value) {
			aMid = firstA;
			bMid = lastB;
			return c;
		}

		firstA = firstA->next;
		lastB = lastB->prev;
	}

	return -1;
}

void join(List a, List b, Node *aMid, Node *bMid) {
	b.fist.prev = aMid;
	a.last.next = bMid;
}

void join(List a, List b) {
	Node *abFirst, *abSecond;
	Node *baFirst, *baSecond;
	int ab = findJoin(&a.fist, &b.last, abFirst, abSecond);
	int ba = findJoin(&b.fist, &a.last, baFirst, baSecond);
	if (ab != -1 && ba != -1) {
		if (ab > ba) {
			join(b, a, baFirst, baSecond);
		} else {
			join(a, b, abFirst, abSecond);
		}
	} else if (ab != -1) {
		join(a, b, abFirst, abSecond);
	} else if (ba != -1) {
		join(b, a, baFirst, baSecond);
	}
}

bool isJoined(Node *first, Node *last) {
	if (!first || !last) {
		return false;
	}
	while (first != last) {
		if (first->next->prev != first) {
			return true;
		}
		first = first->next;
	}
	return false;
}

int sum(Node *first, Node *last) {
	if (!first || !last) {
		return 0;
	}
	int total = 0;
	Node *iter = first;
	while (iter != last) {
		total += iter->value;
		if (iter->next->prev != iter) {
			break;
		}
		iter = iter->next;
	}

	Node *bMid = iter;
	while (iter) {
		total += iter->value;
		iter = iter->next;
	}

	total -= bMid->value;
	while (bMid != first) {
		total += bMid->value;
		if (bMid->prev->next != bMid) {
			break;
		}
	}
}

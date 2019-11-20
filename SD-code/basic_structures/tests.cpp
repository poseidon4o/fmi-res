#include "queue.hpp"

void popAll(Queue<int> q) {
	int c = 0;
	while (!q.isEmpty()) {
		assert(q.front() == c);
		c++;
		q.pop();
	}
}

void testQueue() {
	Queue<int> a;
	{
		Queue<int> que(1);
		Queue<int> p(que);
		que = p;
		for (int c = 0; c < 2500; c++) {
			que.push(c);
			assert(que.front() == 0);
		}

		p = que;
		popAll(que);
		a = que = p;
		p = p;
	}
}

 

int main() {
	testQueue();

	return 0;
}
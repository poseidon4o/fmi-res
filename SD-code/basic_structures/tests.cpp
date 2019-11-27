#include "queue.hpp"
#include "list.hpp"
#include <iostream>

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

struct Student {
	int fn = 3;
	Student(int grade) : fn(grade) {}
	Student() {}
};

void print(const List<Student> copy) {
	std::cout << std::endl;
	for (const Student &a : copy) {
		std::cout << a.fn << ' ';
	}

	List<Student> nonConst;
	nonConst = copy;

	std::cout << std::endl;
	for (List<Student>::const_iterator it = nonConst.cbegin(); it != nonConst.end(); ++it) {
		std::cout << it->fn << ' ';
	}
}

void testList() {
	List<Student> students;
	for (int c = 0; c < 10; c++) {
		students.pushBack(Student(c));
	}

	students.insertAfter(students.begin(), -7);
	students.insertBefore(students.begin(), -5);

	for (List<Student>::iterator it = students.begin(); it != students.end(); ++it) {
		std::cout << it->fn << " ";
		*it = Student(it->fn + 1);
	}

	print(students);

	for (int c = 0; c < 3; c++) {
		students.popFront();
	}

	print(students);

	for (int c = 0; c < 3; c++) {
		students.popBack();
	}

	print(students);

	List<Student> copy(students);
	students.splice(copy);

	for (int c = 0; c < 3; c++) {
		students.remove(students.begin());
	}

	print(students);
}

int main() {
	testQueue();
	testList();

	return 0;
}
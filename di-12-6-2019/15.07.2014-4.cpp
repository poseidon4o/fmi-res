#include <cstring>
#include <cstdio>
#include <iostream>

int move(int index, int dir, int len) {
	return (index + dir + len) % len;
}

bool check(const char *str, int index, int len) {
	int right = index;
	int left = move(index, -1, len);

	for (int c = 0; c < len / 2; c++) {
		if (str[left] != str[right]) {
			return false;
		}
		left = move(left, -1, len);
		right = move(right, 1, len);
	}

	return true;
}

void check(const char *str) {
	int len = strlen(str);

	for (int c = 0; c < len; c++) {
		if (check(str, c, len)) {
			std::cout << c << " (";
			std::cout << (str + c);
			for (int r = 0; r < c; r++) {
				std::cout << str[r];
			}
			std::cout << ')';
			return;
		}
	}

	std::cout << "NO";
}


int main() {
	check("booo");
	getchar();
	return 0;
}
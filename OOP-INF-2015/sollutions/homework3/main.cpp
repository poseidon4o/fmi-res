#include "BoolVector.hpp"
#include "Vector.h"

#include <iostream>
using namespace std;


int main() {
    Vector a(2), b(2);
    a[0] = 1;
    a[1] = 0;

    b[0] = 0;
    b[1] = 1;

    cout << (a + b + a + a + 3 * b - a * 17) % a * a + b / 3;

    cin.get();
    return 0;
}

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
    cout << endl << endl;
    BoolVector bv;
    for (int c = 0; c < 40; ++c) {
        cout << bv[c] << ' ';
        if (c % 2) {
            bv.set(c);
        }
    }
    cout << endl;

    for (int c = 0; c < 40; ++c) {
        cout << bv[c] << ' ';
    }

    cin.get();
    return 0;
}

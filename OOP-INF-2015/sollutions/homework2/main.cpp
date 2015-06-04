#include "Jar.h"

int main() {
    Jar j;
    const char * testStr = "test";
    for (int c = 0; c < 10000; ++c) {
        Cookie cookie;
        for (int r = 0; r < 10; ++r) {
            cookie.addString(testStr);
        }
        j.addCookie(cookie);
    }
    return 0;
}
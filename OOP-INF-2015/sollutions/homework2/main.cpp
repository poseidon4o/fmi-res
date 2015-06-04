#include "Jar.h"
#include <cassert>

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

    int size = j.binarySize();

    char * data = new char[size];
    char * serialize = data;
    j.serialize(serialize);

    // serialize ptr moved exactly size bytes as expected
    assert(data + size == serialize);

    char * deserialize = data;
    Jar j2 = deserializeJar(deserialize);

    // deserialize ptr moved exactly size bytes as expected
    assert(deserialize == data + size);

    // both jars have the same size
    assert(j.getSize() == j2.getSize());

    return 0;
}
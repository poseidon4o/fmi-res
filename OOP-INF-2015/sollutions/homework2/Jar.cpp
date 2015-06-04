#include "Jar.h"
#include <string.h>
#include <new> // std::nothrow
#include <cassert>

Cookie::Cookie() : stringCount(0) {
}

bool Cookie::addString(const char * str) {
    if (this->stringCount == MAX_STRINGS) {
        return false;
    }

    this->strings[this->stringCount] = new (std::nothrow) char[strlen(str) + 1];
    if (!this->strings[this->stringCount]) {
        return false;
    }

    strcpy(this->strings[this->stringCount], str);
    ++this->stringCount;
    return true;
}

Cookie::Cookie(const Cookie & other) : stringCount(0) {
    this->copy(other);
}

void Cookie::copy(const Cookie & other) {
    for (int c = 0; c < other.stringCount; ++c) {
        if (!this->addString(other.strings[c])) {
            break;
        }
    }
}

void Cookie::free() {
    for (int c = 0; c < this->stringCount; ++c) {
        delete[] this->strings[c];
    }
    this->stringCount = 0;
}

Cookie & Cookie::operator=(const Cookie & other) {
    if (this == &other) {
        return *this;
    }
    this->free();
    this->copy(other);
    return *this;
}

Cookie::~Cookie() {
    this->free();
}

Jar::Jar() : size(0), capacity(1) {
    this->cookies = new (std::nothrow) Cookie[this->capacity];
}

Jar::Jar(const Jar & other) : size(0), capacity(0) {
    this->copy(other);
}

Jar & Jar::operator=(const Jar & other) {
    if (this == &other) {
        return *this;
    }
    this->free();
    this->copy(other);
    return *this;
}

Jar::~Jar() {
    this->free();
}

bool Jar::eatCookie() {
    if (this->cookies && this->size) {
        --this->size;
        return resize();
    }
    return false;
}

bool Jar::addCookie(const Cookie & cookie) {
    if (!this->resize()) {
        return false;
    }
    this->cookies[this->size++] = cookie;
    return true;
}

void Jar::free() {
    delete[] this->cookies;
    this->size = this->capacity = 0;
}

bool Jar::copy(const Jar & other) {
    this->cookies = new (std::nothrow) Cookie[other.capacity];
    if (!this->cookies) {
        return false;
    }

    this->capacity = other.capacity;
    this->size = other.size;
    for (int c = 0; c < this->size; ++c) {
        this->cookies[c] = other.cookies[c];
    }
}

bool Jar::resize() {
    if (this->size < this->capacity && this->size >= this->capacity / 2) {
        // everything is ok
        return true;
    }

    Cookie * newCookies = NULL;
    int newCapacity;
    if (this->size == this->capacity) {
        newCookies = new (std::nothrow) Cookie[newCapacity = this->capacity * 2];
    } else if (this->size < this->capacity / 2) {
        Cookie * newCookies = new (std::nothrow) Cookie[newCapacity = this->capacity / 2];
    } else {
        // should not happen
        assert(false);
        return false;
    }

    if (!newCookies) {
        return false;
    }

    this->capacity = newCapacity;

    for (int c = 0; c < this->size; ++c) {
        newCookies[c] = this->cookies[c];
    }

    delete[] this->cookies;
    this->cookies = newCookies;
    return true;
}
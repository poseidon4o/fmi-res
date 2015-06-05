#include "Vector.h"
#include <memory>
#include <algorithm>
#include <iostream>

Vector::Vector(int size): data(new double[size]), size(size) {
    clear();
}

Vector::~Vector() {
    delete[] data;
}

Vector::Vector(const Vector & other): data(new double[other.size]), size(other.size) {
    memcpy(data, other.data, size * sizeof(double));
}

Vector & Vector::operator=(const Vector & other) {
    if (this == &other) {
        return *this;
    }
    delete[] data;
    data = new double[size = other.size];
    memcpy(data, other.data, sizeof(double) * size);
}

void Vector::clear() {
    memset(data, 0, sizeof(double) * size);
}

void Vector::toSize(const Vector & other) {
    if (size >= other.size) {
        return;
    }
    double * newData = new double[other.size];
    memset(newData, 0, sizeof(double) * other.size);
    memcpy(newData, data, sizeof(double) * size);
    delete[] data;
    data = newData;
    size = other.size;
}

double Vector::operator[](int idx) const {
    return (idx >= 0 && idx < size) ? data[idx] : 0.0;
}


double & Vector::operator[](int idx) {
    // ugly but simple
    static double dummy = 0;
    // restore of changed
    dummy = 0;
    return (idx >= 0 && idx < size) ? data[idx] : dummy;
}

Vector Vector::operator-() const {
    return Vector(*this) *= -1;
}

int Vector::length() const {
    double sum = 0.;
    for (int c = 0; c < size; ++c) {
        sum += data[c];
    }
    return sqrt(sum);
}

int Vector::getSize() const {
    return size;
}

Vector & Vector::operator+=(const Vector & other) {
    toSize(other);
    for (int c = 0; c < size; ++c) {
        data[c] += other[c];
    }
    return *this;
}

Vector & Vector::operator-=(const Vector & other) {
    // one copy ctor too much
    return this->operator+=(-other);
}

Vector & Vector::operator*=(double scalar) {
    for (int c = 0; c < size; ++c) {
        data[c] *= scalar;
    }
    return *this;
}

Vector & Vector::operator/=(double scalar) {
    // may lose precision
    return this->operator*=(1 / scalar);
}

Vector operator+(const Vector & left, const Vector & right) {
    return Vector(left) += right;
}

Vector operator-(const Vector & left, const Vector & right) {
    return Vector(left) -= right;
}

Vector operator*(const Vector & left, double scalar) {
    return Vector(left) *= scalar;
}

Vector operator*(double scalar, const Vector & right) {
    return Vector(right) *= scalar;
}

Vector operator/(const Vector & left, double scalar) {
    return Vector(left) /= scalar;
}

double operator%(const Vector & left, const Vector & right) {
    double product = 0.0;
    int compCount = std::max(left.getSize(), right.getSize());
    for (int c = 0; c < compCount; ++c) {
        product += left[c] * right[c];
    }
    return product;
}

std::ostream & operator<<(std::ostream & strm, const Vector & v) {
    strm << "Vector(" << v[0];
    for (int c = 1; c < v.getSize(); ++c) {
        strm << ',' << v[c];
    }
    strm << ')';
    return strm;
}

#ifndef VECTOR_H
#define VECTOR_H

#include <ostream>

class Vector {
public:
    Vector(int size);
    ~Vector();
    Vector(const Vector & other);
    Vector & operator=(const Vector & other);

    double operator[](int idx) const;
    Vector & operator+=(const Vector & other);
    Vector & operator-=(const Vector & other);
    Vector & operator*=(double scalar);
    Vector & operator/=(double scalar);

    // let's not make completely useless Vector class
    double & operator[](int idx);
    Vector operator-() const;
    int length() const;
    int getSize() const;

private:
    void clear();
    void toSize(const Vector & other);
private:
    int size;
    double * data;
};

Vector operator+(const Vector & left, const Vector & right);
Vector operator-(const Vector & left, const Vector & right);

Vector operator*(const Vector & left, double scalar       );
Vector operator*(double scalar      , const Vector & right);

Vector operator/(const Vector & left, double scalar);

double operator%(const Vector & left, const Vector & right);
std::ostream & operator<<(std::ostream & strm, const Vector & v);

#endif // VECTOR_H

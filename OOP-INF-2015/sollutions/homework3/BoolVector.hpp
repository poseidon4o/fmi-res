#ifndef BOOL_VECTOR_HPP
#define BOOL_VECTOR_HPP

class BoolVector {
public:
    BoolVector(): data(0) {}

    int operator[](int idx) {
        return (idx >= 0 && idx < 32) ? (data >> idx) & 1 : -1;
    }

    bool set(int idx) {
        if (this->operator[](idx) != -1) {
            data |= (1 << idx);
            return true;
        }
        return false;
    }

    bool clear(int idx) {
        if (this->operator[](idx) != -1) {
            data &= ~(1 << idx);
            return true;
        }
        return false;
    }

private:
    unsigned int data;
};

#endif // BOOL_VECTOR_HPP

#ifndef CLU_FILTER_CHAI_H
#define CLU_FILTER_CHAI_H

#include "filter.h"

/*
    Filter chain consisting of dynamically resizing array of
    Filter objects, and pointers to input and output.

    Each object is linked to the previous one except the first one,
    which is linked to the input source.

    This will allow pulling line from the last filter and it will propagate
    trough the chain until it reaches the source. On the way back only lines
    passing each filter will be returned.
*/

class FilterChain {
    Filter ** filters;
    int size, capacity;

    istream * input;
    ostream * output;
public:
    // ctors
    FilterChain(istream &, ostream &);
    FilterChain(const FilterChain &);
    FilterChain & operator=(const FilterChain &);

    ~FilterChain();

    // serialization/deserialization
    static FilterChain deserialize(const char *, istream &, ostream &);
    void serialize(const char *);

    // add filter to the back of the chain
    void add_filter(const Filter &);

    // remove last added filter
    void pop_filter();

    // remove filter by the "word" it's filtering
    void remove_filter_word(const char *);

    // actual filter method which will filter the data
    void filter();
private:
    // helper method to reconnect al parts of the chain
    void reconnect();

    void copy(const FilterChain &);
    void free();

    // helper methods for resizing
    void grow();
    bool has_space();
};

#endif // CLU_FILTER_CHAI_H

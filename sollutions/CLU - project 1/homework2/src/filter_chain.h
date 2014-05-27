#ifndef CLU_FILTER_CHAI_H
#define CLU_FILTER_CHAI_H

#include "filter.h"

class FilterChain {
    Filter ** filters;
    size_t size, capacity;

    istream * input;
    ostream * output;
public:
    FilterChain(istream &, ostream &);
    FilterChain(const FilterChain &);
    FilterChain operator=(const FilterChain &);

    ~FilterChain();

    static FilterChain unserialize(const char *, istream &, ostream &);
    void serialize(const char *);

    void add_filter(const Filter &);
    void pop_filter();
    void remove_filter_word(const char *);

    void filter();
private:
    void reconnect();

    void copy(const FilterChain &);
    void free();

    void grow();
    bool has_space();
};

#endif // CLU_FILTER_CHAI_H

#include "filter_chain.h"
#include <cstring>

FilterChain::FilterChain(istream & in, ostream & out)
    : input(&in), output(&out), filters(NULL), size(0), capacity(8)
{
    grow();
}

FilterChain::FilterChain(const FilterChain & other)
    : input(other.input), output(other.output), filters(NULL)
{
    copy(other);
}

FilterChain FilterChain::operator=(const FilterChain & other) {
    if(this == &other) {
        return *this;
    }
    free();
    copy(other);
    return *this;
}

FilterChain::~FilterChain() {
    free();
}

void FilterChain::add_filter(const Filter & filter) {
    if(!has_space()) {
        grow();
    }
    filters[size++] = new(nothrow) Filter(filter);
    if(!filters[size-1]) {
        --size;
    }
}
void FilterChain::pop_filter() {
    if(size) {
        delete filters[size--];
    }
}

void FilterChain::remove_filter_word(const char * word) {
    for(size_t c = 0; c < size; ++c) {
        if(!strcmp(word, filters[c]->get_word())) {
            swap(filters[c], filters[--size]);
            delete filters[size+1];
        }
    }
}

void FilterChain::reconnect() {
    if(size) {
        filters[0]->set_source(*input);
        for(size_t c = size-1; c > 0; --c) {
            filters[c]->set_source(*filters[c-1]);
        }
    }
}

void FilterChain::filter() {
    reconnect();
    if(size) {
        char * line = NULL;
        Filter & last = *filters[size-1];
        do {
            delete[] line;
            last.next_line(line);
            if(line) {
                (*output) << line << endl;
            }
        } while(line);
        delete[] line;
    }
}

void FilterChain::copy(const FilterChain & other) {
    size = other.size;
    capacity = other.capacity;
    filters = new Filter*[size];

    for(size_t c = 0; c < size; ++c) {
        filters[c] = new Filter(*other.filters[c]);
    }
}

void FilterChain::free() {
    for(size_t c = 0; c < size; ++c) {
        delete filters[c];
    }
    delete[] filters;
    size = capacity = 0;
}

void FilterChain::grow() {
    Filter ** grown = new Filter*[capacity *= 2];
    memset(grown, 0, sizeof(Filter*) * capacity);
    memcpy(grown, filters, size * sizeof(Filter*));
    delete[] filters;
    filters = grown;
}

bool FilterChain::has_space() {
    return size < capacity;
}


FilterChain FilterChain::unserialize(const char * file_name, istream & input, ostream & output) {
    FilterChain chain(input, output);

    ifstream file(file_name, ios::binary);
    if(!file) {
        return chain;
    }

    int size = 0;
    file.read((char*)&size, sizeof(size));

    for(int c = 0; c < size; ++c) {
        chain.add_filter(Filter::unserialize(file));
    }
    file.close();

    return chain;
}
void FilterChain::serialize(const char * file_name) {
    ofstream file(file_name, ios::binary);
    if(!file) {
        return;
    }

    file.write((char*)&size, sizeof(size));
    for(int c = 0; c < size; ++c) {
        filters[c]->serialize(file);
    }
    file.close();
}

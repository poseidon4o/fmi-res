#ifndef CLU_FILTER_H
#define CLU_FILTER_H

#include <iostream>
#include <fstream>
using namespace std;

/*
    Filter class containing "word" which it will filter and pointer
    to source.

    Source can be another filter or istream.
*/

class Filter {
    Filter * f_source;
    istream * s_source;
    enum SourceType {NONE, STREAM, FILTER} source_type;

    char * word;
public:
    // ctors for word, and source
    Filter(const char * = NULL);
    Filter(const char *, Filter &);
    Filter(const char *, istream &);

    Filter(const Filter &);
    Filter & operator=(const Filter &);

    ~Filter();

    // serialization/deserialization
    static Filter deserialize(ifstream &);
    void serialize(ofstream &);

    // set/get methods
    void set_word(const char *);
    const char * get_word() const;

    void set_source(Filter &);
    void set_source(istream &);

    // interface to pull next line that passes trough,
    // NULL assigned if reached EOF
    void next_line(char *&);

private:
    void copy(const Filter &);
    void next_filter_line(char *&); // helper to return next line if source is filter
    void next_stream_line(char *&); // helper for stream source
};

#endif // CLU_FILTER_H

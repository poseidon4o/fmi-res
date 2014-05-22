#ifndef CLU_FILTER_H
#define CLU_FILTER_H

#include <iostream>

using namespace std;

class Filter {
    Filter * f_source;
    istream * s_source;
    enum SourceType {STREAM, FILTER} source_type;

    char * word;
public:
    Filter(const char *, Filter &);
    Filter(const char *, istream &);

    Filter(const Filter &);
    Filter operator=(const Filter &);

    ~Filter();

    void set_word(const char *);
    const char * get_word() const;

    void set_source(Filter &);
    void set_source(istream &);

    void next_line(char *&);

private:
    void copy(const Filter &);
    void next_filter_line(char *&);
    void next_stream_line(char *&);
};

#endif // CLU_FILTER_H

#include "filter.h"
#include <cstring>

#define CHUNK 1024

Filter::Filter(const char * word)
    :word(NULL), s_source(NULL), f_source(NULL), source_type(NONE)
{
    set_word(word);
}

Filter::Filter(const char * word, Filter & src)
    :word(NULL), s_source(NULL), f_source(NULL), source_type(NONE)
{
    set_word(word);
    set_source(src);
}

Filter::Filter(const char * word, istream & src)
    :word(NULL), s_source(NULL), f_source(NULL), source_type(NONE)
{
    set_word(word);
    set_source(src);
}

Filter::Filter(const Filter & other)
    :word(NULL), s_source(NULL), f_source(NULL), source_type(NONE)
{
    copy(other);
}

Filter Filter::operator=(const Filter & other) {
    if(this == &other) {
        return *this;
    }
    copy(other);
    return *this;
}

void Filter::copy(const Filter & other) {
    set_word(other.word);
    if(other.source_type == FILTER) {
        set_source(*other.f_source);
    } else if(other.source_type == STREAM) {
        set_source(*other.s_source);
    } else {
        f_source = NULL;
        s_source = NULL;
        source_type = NONE;
    }
}

Filter::~Filter() {
    delete[] word;
}

const char * Filter::get_word() const {
    return word;
}

void Filter::set_word(const char * word) {
    delete[] this->word;
    this->word = NULL;
    if(word) {
        this->word = new(nothrow) char[strlen(word)+1];
        if(!this->word) {
            return;
        }

        strcpy(this->word, word);
    }

}

void Filter::set_source(Filter & src) {
    source_type = FILTER;
    f_source = &src;
    s_source = NULL;
}

void Filter::set_source(istream & src) {
    source_type = STREAM;
    f_source = NULL;
    s_source = &src;
}

void Filter::next_line(char *& line) {
    if(source_type == FILTER) {
        next_filter_line(line);
    } else if(source_type == STREAM) {
        next_stream_line(line);
    } else {
        line = NULL;
    }
}

void Filter::next_filter_line(char *& line) {
    do {
        f_source->next_line(line);
    } while(line && !strstr(line, word));
}

void Filter::next_stream_line(char *& line) {
    if(s_source->eof()) {
        line = NULL;
        return;
    }

    size_t line_len = CHUNK;
    line = new(nothrow) char[line_len];
    if(!line) {
        return;
    }
    line[0] = '\0';

    char buff[CHUNK];
    do {
        s_source->clear();
        s_source->getline(buff, CHUNK);
        if(strlen(line) + s_source->gcount() > line_len) {
            char * tmp = new(nothrow) char[line_len *= 2];
            if(!tmp) {
                delete[] line;
                line = NULL;
                return;
            }

            strcpy(tmp, line);
        }
        strcat(line, buff);
    } while(s_source->gcount() == CHUNK-1);

    if(word && strstr(line, word)) {
        return;
    } else {
        delete[] line;
        line = NULL;
        next_stream_line(line);
    }
}

void Filter::serialize(ofstream & file) {
    int word_len = strlen(word);
    file.write((char*)&word_len, sizeof(word_len));
    file.write(word, word_len);
}

Filter Filter::unserialize(ifstream & file) {
    int word_len = 0;
    file.read((char*)&word_len, sizeof(word_len));
    if(!word_len) {
        return Filter();
    }

    char * buff = new char[word_len+1];
    memset(buff, 0, word_len+1);

    file.read(buff, word_len);
    Filter deserialized(buff);

    delete[] buff;
    return deserialized;
}

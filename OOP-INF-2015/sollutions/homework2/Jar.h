#ifndef JAR_H
#define JAR_H

const int MAX_STRINGS = 20;

class Cookie {
public:
    Cookie();
    bool addString(const char * str);

    Cookie(const Cookie & other);
    Cookie & operator=(const Cookie & other);
    ~Cookie();

    int binarySize();
    void serialize(char *& data);
    friend Cookie deserializeCookie(char *& data);

private:
    void copy(const Cookie & other);
    void free();
private:
    char * strings[MAX_STRINGS];
    int stringCount;
};

class Jar {
public:

    Jar();
    Jar(const Jar & other);
    Jar & operator=(const Jar & other);
    ~Jar();

    bool eatCookie();
    bool addCookie(const Cookie & cookie);

    int getSize() const;

    int binarySize();
    void serialize(char *& data);
    friend Jar deserializeJar(char *& data);

private:
    void free();
    bool copy(const Jar & other);
    bool resize();
private:
    Cookie * cookies;
    int size, capacity;
};

#endif JAR_H
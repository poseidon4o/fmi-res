#include "filter.h"
#include "filter_chain.h"
#include <fstream>
#include <cstring>

using namespace std;

bool prompt_user(const char *);
char * read_line(const char *);

int main(int argc, char * argv[]) {
    istream * input = NULL;
    ostream * output = NULL;

    ifstream input_file;
    ofstream output_file;

    // 2 or 3 arguments means there are input/output file
    if(argc == 2 || argc == 3) {

        // open input file
        input_file.open(argv[1], ios::in);
        if(!input_file.is_open()) {
            cerr << "Can't open input file [" << argv[1] << "]\nError: " << strerror(errno);
            return -1;
        }
        input = &input_file; // assign file to input ptr

        // 3 arguments - open output file
        if(argc == 3) {
            output_file.open(argv[2]);
            if(!output_file.is_open()) {
                cerr << "Can't open output file [" << argv[2] << "]\nError: " << strerror(errno);
                return -1;
            }
            output = &output_file; // assign file to output ptr
        } else {
            output = &cout; // default to cout
        }
    } else {
        input = &cin; // default to cin
        output = &cout; // default to cout
    }

    FilterChain chain(*input, *output);

    if(prompt_user("Add filters? Y/N: ")) {
        do {
            char * line = read_line("Enter filter word: ");
            chain.add_filter(line);
            delete[] line;
        } while(prompt_user("Add more filters? Y/N: "));
    }

    if(prompt_user("Serialize created chain? Y/N: ")) {
        char * name = read_line("Enter output name: ");
        chain.serialize(name ? name : "filter-chain.dat");
        delete[] name;
    }

    if(prompt_user("Load chain from file? Y/N: ")) {
        char * name = read_line("Enter file name: ");
        chain = FilterChain::deserialize(name ? name : "filter-chain.dat", *input, *output);
        delete[] name;
    }

    chain.filter(); // actual filtering

    return 0;
}

// yes/no prompt
bool prompt_user(const char * prompt) {
    cout << prompt;
    char buff[64];
    cin >> buff;
    return !strcmp(buff, "Y") || !strcmp(buff, "y") || !strcmp(buff, "y");
}

// read arbitrary long line
char * read_line(const char * message) {
    cin.sync();
    cin.clear();
    cout << message;

    int length = 64;
    char * line = new(nothrow) char[length];
    if(!line) {
        return line;
    }
    memset(line, 0, length);

    int chunk = length;
    char buff[chunk];
    do {
        cin.clear();
        cin.getline(buff, chunk);
        if(strlen(line) + strlen(buff) > length) {
            char * repl = new(nothrow) char[length *= 2];
            if(!repl) {
                delete[] line;
                return NULL;
            }
            strcpy(repl, line);
            delete[] line;
            line = repl;
        }
        strcat(line, buff);
    } while(cin.gcount() == chunk - 1);

    return line;
}

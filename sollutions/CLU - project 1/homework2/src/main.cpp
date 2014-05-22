#include "filter.h"
#include <fstream>

using namespace std;

int main() {
    ifstream inp("test.txt", ios::in);
    Filter filter("random", inp);
    char * line;

    do {
        filter.next_line(line);
        if(line) {
            cout << line << endl;
        }
        delete[] line;
    } while(line);

    inp.close();
    return 0;
}

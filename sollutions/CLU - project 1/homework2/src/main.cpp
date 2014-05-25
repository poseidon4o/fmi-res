#include "filter.h"
#include "filter_chain.h"
#include <fstream>

using namespace std;

int main() {
    const char * file_name = "numbers.txt";

    ifstream file(file_name);
    if(!file) {
        cerr << "Failed to open file!";
        return -1;
    }

    FilterChain chain(file, cout);

    chain.add_filter("eight"); // implicit
    chain.add_filter("three");
    chain.add_filter(Filter("seven")); // explicit

    chain.filter();

    chain.serialize("chain.dat");


    file.close();
    file.clear();
    file.open(file_name);
    if(!file) {
        cerr << "Failed to reopen file";
        return -1;
    }

    cout << "\n-----------------------------------------\n";

    FilterChain loaded(file, cout);

    loaded.unserialize("chain.dat");
    loaded.filter();


    return 0;
}

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

    Filter seven("seven");

    ofstream serialized_file("seven.dat", ios::binary);
    seven.serialize(serialized_file);
    serialized_file.flush();
    serialized_file.close();

    ifstream unserialize_file("seven.dat", ios::binary);
    Filter seven_new(Filter::unserialize(unserialize_file));
    unserialize_file.close();

    chain.add_filter(seven_new);

    chain.filter();

    return 0;
}

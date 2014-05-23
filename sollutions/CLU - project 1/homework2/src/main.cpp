#include "filter.h"
#include "filter_chain.h"
#include <fstream>

using namespace std;

int main() {
    ifstream file("test.txt");

    FilterChain chain(file, cout);
    chain.add_filter("eight");
    chain.add_filter("nine");
    chain.add_filter("seven");

    chain.filter();
    return 0;
}

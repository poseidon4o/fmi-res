#include <iostream>
#include <cstring>
using namespace std;

void p(const string & out) { cout << out << endl; }

int main() {
    string inp;
    while(true) {
        getline(cin, inp);
        if(inp == "exit") {
            p("exit");
            return 0;
        } else if(inp.find("move") != string::npos) {
            p(inp);
        } else if(inp.find("status") != string::npos) {
            p("playing white");
        } else if(inp.find("display") != string::npos) {
            p("display");
            for(int c = 0; c < 8; ++c) {
                for(int r = 0; r < 8; ++r) {
                    p(
                      string("") + (char)('A' + c) + string(" ") + (char)('0' + r + 1) + string(" ") +
                      (r % 2 ? "white" : "black") + string(" ") +
                      (c % 2 ? "pawn" : "queen"));
                }
            }
        }
    }
    return 0;
}

#include <stack>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <functional>
#include <iterator>
#include <random>

struct op {
    char m_symbol, m_operator;
    int m_priority, m_associative;
};

using namespace std;

vector<string> split_space(const string & str) {
    return vector<string>{ istream_iterator<string>(stringstream(str)), istream_iterator<string>() };
}


// calculations

double do_calc(op & operation, double left, double right) {
    if (operation.m_operator == '/' && right == 0.0) {
        throw runtime_error("Division by zero " + to_string(left) + "/0");
    }
    switch (operation.m_operator) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/': return left / right;
    }
    throw runtime_error("Unknown operation!");
}

void empty_stack(
    stack<op> & ops,
    stack<double> & vals,
    function<bool(const stack<op> &)> pred = [](const stack<op> &) { return true; },
    const op & cur = { -1, -1, -1, -1 }
) {
    while (!ops.empty() && pred(ops)) {
        if (ops.top().m_priority == cur.m_priority && ops.top().m_associative != cur.m_associative) {
            throw runtime_error("Two operators with same priority and different associativity " + string(1, ops.top().m_symbol) + " == " + string(1, cur.m_symbol));
        }
        double right = vals.top(); vals.pop();
        double left = vals.top(); vals.pop();
        vals.push(do_calc(ops.top(), left, right));
        ops.pop();
    }
}

double calc_expression(const string & expr, op table[256]) {
    vector<string> tokens = split_space(expr);

    stack<op> ops;
    stack<double> vals;

    for (const auto & tok : tokens) {
        if (tok == "(") {
            ops.push({ '(', -1, -1, -1 });
        } else if (tok == ")") {
            empty_stack(ops, vals, [](const stack<op> & ops) { return ops.top().m_symbol != '('; });
            ops.pop();
        } else if (table[tok[0]].m_symbol == 0) {
            vals.push(stoi(tok));
        } else {
            op & cur = table[tok[0]];
            if (ops.empty()) {
                ops.push(cur);
            } else if (cur.m_priority > ops.top().m_priority) {
                ops.push(cur);
            } else {
                if (cur.m_priority == ops.top().m_priority && cur.m_priority == 1 && ops.top().m_priority == 1) {
                    ops.push(cur);
                } else {
                    empty_stack(ops, vals, [&cur](const stack<op> & ops) { return cur.m_priority <= ops.top().m_priority; }, cur);
                    ops.push(cur);
                }
            }
        }
    }
    empty_stack(ops, vals);
    return vals.top();
}


// generators
vector<string> gen_expr(int reps);
vector<string> gen_table();


int main(int argc, char * argv[]) {
    vector<string> & table_str = gen_table();
    vector<string> & expr_str = gen_expr(100);

    op table[256];
    memset(table, 0, sizeof(op)* 256);

    string line;
    for (const auto & line : table_str) {
        cout << line;
        vector<string> parts = split_space(line);
        table[parts[0][0]] = { parts[0][0], parts[1][0], stoi(parts[2]), (parts[3][0] == '1') };
    }
    cout << endl << endl;
    for (const auto & line : expr_str) {
        double res;
        try {
            res = calc_expression(line, table);
        } catch (runtime_error & e) {
            cout << line << endl << "error " << e.what() << endl;
            continue;
        }
        cout << line << endl;
        cout.precision(15);
        cout << fixed << res << endl;
    }

    return 0;
}




vector<string> gen_expr(int reps) {
    random_device dev;
    mt19937 gen;
    uniform_int<int> items(20, 100);
    uniform_int<int> vals(-5000, 5000);
    uniform_int<char> ops('a', 'z');
    uniform_int<int> braces(0, 100);
    vector<string> res;

    while (reps--) {
        stringstream line;
        int parts = items(gen);
        bool need_num = true;
        int open_br = 0;
        for (int c = 0; c < parts; ++c) {
            if (need_num) {
                line << ' ' << vals(gen);
                need_num = false;
                if (braces(gen) < 25 && open_br > 0) {
                    line << " )";
                    open_br -= 1;
                }
            } else {
                line << ' ' << ops(gen);
                need_num = true;
                if (braces(gen) < 40) {
                    line << " (";
                    open_br += 1;
                }
            }
        }
        if (need_num) {
            line << ' ' << vals(gen);
        }
        while (open_br--) {
            line << " )";
        }
        res.push_back(line.str());
    }
    return res;
}

vector<string> gen_table() {
    random_device dev;
    mt19937 gen;
    uniform_int<int> priority(1, 40);
    uniform_int<int> assoc(0, 100);
    char ops[4] = { '+', '-', '*', '/' };
    uniform_int<int> op(0, 3);

    vector<string> res;

    for (char c = 'a'; c <= 'z'; ++c) {
        stringstream line;
        line << c << ' ' << ops[op(gen)] << ' ' << priority(gen) << ' ' << (assoc(gen) < 75) << endl;
        res.push_back(line.str());
    }
    return res;
}
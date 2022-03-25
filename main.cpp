#include "expression_tree.h"
#include "expression.h"
#include "calculator.h"

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

using namespace std;

string read_and_validate_new_var(istream& in) {
    if (in.eof()) {
        throw invalid_argument("Variable name must not be empty");
    }
    string name;
    in >> name >> ws;
    if (!in.eof()) {
        throw invalid_argument("Variable name must not contain spaces");
    }
    if (!isalpha(name[0])) {
        throw invalid_argument("Variable name must start with a letter");
    }
    return name;
}

optional<string> read_and_validate_existing_var(
    istream& in, const Calculator& calc
) {
    if (in.eof()) {
        return nullopt;
    }
    string name;
    in >> name >> ws;
    if (!in.eof()) {
        throw invalid_argument("Variable name must not contain spaces");
    }
    if (!calc.var_exists(name)) {
        throw invalid_argument("No variable with name: " + name);
    }
    return name;
}

int main() {
    Calculator calc;
    
    string str;
    string command;
    
    cout << setprecision(6);
    
    while (getline(cin, str)) {
        stringstream ss(str);
        ss >> command >> ws;
        transform(command.begin(), command.end(), command.begin(), [](char c) {
            return toupper(static_cast<unsigned char>(c));
        });
        try {
            if (command == "EXPR") {
                calc.new_expr(build_expression_tree(
                    infix_to_postfix(parse_into_tokens(ss))
                ));
            } else if (command == "SAVE") {
                if (calc.get() == nullptr) {
                    throw invalid_argument("Enter expression");
                }
                calc.save(read_and_validate_new_var(ss));
            } else if (command == "DER") {
                if (calc.get() == nullptr) {
                    throw invalid_argument("Enter expression");
                }
                auto name = read_and_validate_existing_var(ss, calc);
                if (!name.has_value()) {
                    cout << calc.derivative().get() << endl;
                } else {
                    cout << calc.derivative(*name).get() << endl;
                }
            } else if (command == "EVAL") {
                if (calc.get() == nullptr) {
                    throw invalid_argument("Enter expression");
                }
                if (isdigit(ss.peek())) {
                    double x;
                    ss >> x;
                    if (!ss.eof() && ss.peek() != ' ') {
                        throw invalid_argument("Variable name must not start with a digit");
                    }
                    ss >> ws;
                    if (!ss.eof()) {
                        throw invalid_argument("Invalid query");
                    }
                    cout << calc.evaluate(x) << endl;
                } else {
                    string name;
                    ss >> name >> ws;
                    if (!isdigit(ss.peek())) {
                        throw invalid_argument("Invalid query");
                    }
                    double x;
                    ss >> x >> ws;
                    if (!ss.eof()) {
                        throw invalid_argument("Invalid query");
                    }
                    if (!calc.var_exists(name)) {
                        throw invalid_argument("No variable with name: " + name);
                    }
                    cout << calc.evaluate(name, x) << endl;
                }
            } else if (command == "PRINT") {
                if (calc.get() == nullptr) {
                    throw invalid_argument("Enter expression");
                }
                auto name = read_and_validate_existing_var(ss, calc);
                if (!name.has_value()) {
                    cout << calc.get().get() << endl;
                } else {
                    cout << calc.get(*name).get() << endl;
                }
            } else {
                throw invalid_argument("Invalid command");
            }
        } catch (invalid_argument& e) {
            cout << e.what() << endl;
        }
    }
    return 0;
}

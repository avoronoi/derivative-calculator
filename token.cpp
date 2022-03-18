#include "token.h"

#include <variant>
#include <optional>

using namespace std;

ostream& operator<<(ostream& out, const UnaryFunc& func) {
    switch (func) {
        case UnaryFunc::SIN:
            return out << "sin";
        case UnaryFunc::COS:
            return out << "cos";
        case UnaryFunc::TAN:
            return out << "tan";
        case UnaryFunc::COT:
            return out << "cot";
        case UnaryFunc::NEG:
            return out << '-';
        case UnaryFunc::LN:
            return out << "ln";
    };
}
ostream& operator<<(ostream& out, const Brace& brace) {
    switch (brace) {
        case Brace::OPEN:
            return out << '(';
        case Brace::CLOSE:
            return out << ')';
    };
}
ostream& operator<<(ostream& out, const Variable& var) {
    return out << 'x';
}

optional<double> try_make_constant(istream& in) {
    if (isdigit(in.peek())) {
        double d;
        in >> d;
        return d;
    }
    return nullopt;
}
optional<Variable> try_make_variable(istream& in) {
    if (in.get() == 'x' && (in.eof() || !isalpha(in.peek()))) {
        return Variable();
    }
    in.unget();
    return nullopt;
}
BinaryOp::Ptr try_make_binary_op(istream& in) {
    switch (in.get()) {
        case '+':
            return make_unique<BinaryOp::Sum>();
        case '-':
            return make_unique<BinaryOp::Diff>();
        case '*':
            return make_unique<BinaryOp::Mult>();
        case '/':
            return make_unique<BinaryOp::Div>();
        case '^':
            return make_unique<BinaryOp::Pow>();
        default:
            in.unget();
            return nullptr;
    }
}
optional<UnaryFunc> try_make_unary_func(istream& in) {
    string name = "";
    while (!in.eof() && isalpha(in.peek())) {
        name += in.get();
    }
    if (name == "sin") {
        return UnaryFunc::SIN;
    } else if (name == "cos") {
        return UnaryFunc::COS;
    } else if (name == "tan") {
        return UnaryFunc::TAN;
    } else if (name == "cot") {
        return UnaryFunc::COT;
    } else if (name == "ln") {
        return UnaryFunc::LN;
    }
    for (int i = 0; i < name.size(); i++) {
        in.unget();
    }
    return nullopt;
}
optional<Brace> try_make_brace(istream& in) {
    switch (in.get()) {
        case '(':
            return Brace::OPEN;
        case ')':
            return Brace::CLOSE;
        default:
            in.unget();
            return nullopt;
    }
}

bool Token::operator==(const BinaryOp::Type& type) const {
    return std::holds_alternative<BinaryOp::Ptr>(*this)
        && std::get<BinaryOp::Ptr>(*this)->get_type() == type;
}

istream& operator>>(istream& in, Token& token) {
    if (auto d = try_make_constant(in); d) {
        token = *d;
        return in;
    }
    if (auto var = try_make_variable(in); var) {
        token = *var;
        return in;
    }
    if (auto op = try_make_binary_op(in); op) {
        token = move(op);
        return in;
    }
    if (auto func = try_make_unary_func(in); func) {
        token = *func;
        return in;
    }
    if (auto brace = try_make_brace(in); brace) {
        token = *brace;
        return in;
    }
    
    assert(!in.eof());
    
    string invalid_token;
    if (!isalpha(in.peek())) {
        invalid_token += in.peek();
    }
    while (!in.eof() && isalpha(in.peek())) {
        invalid_token += in.get();
    }
    throw invalid_argument("Invalid token: " + invalid_token);
}
ostream& operator<<(ostream& out, const Token& token) {
    if (holds_alternative<double>(token)) {
        return out << get<double>(token);
    }
    if (holds_alternative<Variable>(token)) {
        return out << get<Variable>(token);
    }
    if (holds_alternative<Brace>(token)) {
        return out << get<Brace>(token);
    }
    if (holds_alternative<UnaryFunc>(token)) {
        return out << get<UnaryFunc>(token);
    }
    if (holds_alternative<BinaryOp::Ptr>(token)) {
        return out << *get<BinaryOp::Ptr>(token);
    }
    throw logic_error("Unreachable code");
}

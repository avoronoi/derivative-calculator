#pragma once

#include "binary_operation.h"

#include <sstream>
#include <variant>

enum UnaryFunc {
    SIN, COS, TAN, COT, NEG, LN
};
std::ostream& operator<<(std::ostream& out, const UnaryFunc& func);

enum Brace {
    OPEN, CLOSE
};
std::ostream& operator<<(std::ostream& out, const Brace& brace);

struct Variable {};
std::ostream& operator<<(std::ostream& out, const Variable& var);

using BaseToken = std::variant<double, Variable, Brace, UnaryFunc, BinaryOp::Ptr>;
class Token : public BaseToken {
    using BaseToken::BaseToken;

public:
    template<typename T>
    bool operator!=(const T& val) const {
        return !operator==(val);
    }
    
    template<typename T>
    bool operator==(const T& val) const {
        return std::holds_alternative<T>(*this) && std::get<T>(*this) == val;
    }
    
    bool operator==(const BinaryOp::Type& op) const;
};

std::istream& operator>>(std::istream& in, Token& token);
std::ostream& operator<<(std::ostream& out, const Token& token);

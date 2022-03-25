
#include "expression.h"
#include "expression_tree.h"

#include <vector>
#include <istream>
#include <variant>
#include <stdexcept>

using namespace std;

vector<Token> parse_into_tokens(istream& in) {
    vector<Token> ret;
    for (Token token; !(in >> ws).eof(); ) {
        in >> token;
//        convert binary minus to unary if it follows an opening brace
        if (token == BinaryOp::Type::DIFF
            && (ret.empty() || ret.back() == Brace::OPEN)) {
            ret.push_back(UnaryFunc::NEG);
        } else {
            ret.push_back(move(token));
        }
    }
    return ret;
}

vector<Token> infix_to_postfix(vector<Token> expr) {
//    implements shunting-yard algorithm
    vector<Token> ret;
    vector<Token> stack;
    for (Token& token : expr) {
        if (holds_alternative<double>(token)
            || holds_alternative<Variable>(token)) {
            ret.push_back(move(token));
        } else if (holds_alternative<UnaryFunc>(token)) {
            stack.push_back(move(token));
        } else if (holds_alternative<BinaryOp::Ptr>(token)) {
            using namespace BinaryOp;
            while (!stack.empty() && holds_alternative<Ptr>(stack.back())) {
                const Base& stack_op = *get<Ptr>(stack.back());
                const Base& token_op = *get<Ptr>(token);
                if (stack_op.get_priority() > token_op.get_priority()
                    || (stack_op.get_priority() == token_op.get_priority()
                        && token_op.is_left_assoc())) {
                    ret.push_back(move(stack.back()));
                    stack.pop_back();
                } else {
                    break;
                }
            }
            stack.push_back(move(token));
        } else if (holds_alternative<Brace>(token)) {
            switch (get<Brace>(token)) {
                case Brace::OPEN:
                    stack.push_back(move(token));
                    break;
                case Brace::CLOSE:
                    while (!stack.empty() && stack.back() != Brace::OPEN) {
                        ret.push_back(move(stack.back()));
                        stack.pop_back();
                    }
                    if (stack.empty()) {
                        throw invalid_argument("Invalid expression");
                    }
                    stack.pop_back();
                    if (!stack.empty()
                        && holds_alternative<UnaryFunc>(stack.back())) {
                        ret.push_back(move(stack.back()));
                        stack.pop_back();
                    }
                    break;
            }
        } else {
            throw logic_error("Unreachable code");
        }
    }
    while (!stack.empty()) {
        if (stack.back() == Brace::OPEN) {
            throw invalid_argument("Invalid expression");
        }
        ret.push_back(move(stack.back()));
        stack.pop_back();
    }
    return ret;
}

Node::Ptr build_expression_tree(const vector<Token>& expr) {
    vector<Node::Ptr> stack;
    for (const Token& token : expr) {
        if (holds_alternative<double>(token)) {
            stack.push_back(make_unique<Node::Constant>(get<double>(token)));
        } else if (holds_alternative<Variable>(token)) {
            stack.push_back(make_unique<Node::Variable>());
        } else if (holds_alternative<UnaryFunc>(token)) {
            if (stack.empty()) {
                throw invalid_argument("Invalid expression");
            }
            
            using namespace Node::UnaryFunc;
            switch (get<UnaryFunc>(token)) {
                case UnaryFunc::SIN:
                    stack.back() = make_unique<Sin>(move(stack.back()));
                    break;
                case UnaryFunc::COS:
                    stack.back() = make_unique<Cos>(move(stack.back()));
                    break;
                case UnaryFunc::TAN:
                    stack.back() = make_unique<Tan>(move(stack.back()));
                    break;
                case UnaryFunc::COT:
                    stack.back() = make_unique<Cot>(move(stack.back()));
                    break;
                case UnaryFunc::NEG:
                    stack.back() = make_unique<Neg>(move(stack.back()));
                    break;
                case UnaryFunc::LN:
                    stack.back() = make_unique<Ln>(move(stack.back()));
                    break;
            }
        } else if (holds_alternative<BinaryOp::Ptr>(token)) {
            if (stack.size() < 2) {
                throw invalid_argument("Invalid expression");
            }
            
            Node::Ptr left = move(stack[stack.size() - 2]);
            Node::Ptr right = move(stack.back());
            stack.pop_back();
            
            using BinaryOp::Type;
            using namespace Node::BinaryOp;
            
            switch (get<BinaryOp::Ptr>(token)->get_type()) {
                case Type::SUM:
                    stack.back() = make_unique<Sum>(move(left), move(right));
                    break;
                case Type::DIFF:
                    stack.back() = make_unique<Diff>(move(left), move(right));
                    break;
                case Type::MULT:
                    stack.back() = make_unique<Mult>(move(left), move(right));
                    break;
                case Type::DIV:
                    stack.back() = make_unique<Div>(move(left), move(right));
                    break;
                case Type::POW:
                    stack.back() = make_unique<Pow>(move(left), move(right));
                    break;
            }
        } else {
            throw logic_error("Unreachable code");
        }
    }
    if (stack.size() != 1) {
        throw invalid_argument("Invalid expression");
    }
    return stack.back()->make_simplified();
}

Node::Ptr derivative(const Node::Base* expr) {
    return expr->derivative();
}

std::ostream& operator<<(std::ostream& out, const Node::Base* expr) {
    expr->print(out);
    return out;
}

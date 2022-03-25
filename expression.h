#pragma once

#include "token.h"
#include "expression_tree.h"

#include <vector>
#include <istream>

std::vector<Token> parse_into_tokens(std::istream& in);
std::vector<Token> infix_to_postfix(std::vector<Token> expr);
Node::Ptr build_expression_tree(const std::vector<Token>& expr);
Node::Ptr derivative(const Node::Base* expr);

std::ostream& operator<<(std::ostream& out, const Node::Base* expr);

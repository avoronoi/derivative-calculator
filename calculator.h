#pragma once

#include "expression_tree.h"

#include <unordered_map>

class Calculator {
public:
    void new_expr(std::shared_ptr<Node::Base> expr);
    void save(const std::string& name);
    std::shared_ptr<Node::Base> derivative();
    std::shared_ptr<Node::Base> derivative(const std::string& name);
    double evaluate(double x) const;
    double evaluate(const std::string& name, double x) const;
    std::shared_ptr<Node::Base> get();
    std::shared_ptr<Node::Base> get(const std::string& name);
    bool var_exists(const std::string& name) const;
private:
    std::shared_ptr<Node::Base> last_;
    std::unordered_map<std::string, std::shared_ptr<Node::Base>> vars_;
};

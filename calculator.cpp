#include "calculator.h"

using namespace std;

void Calculator::new_expr(shared_ptr<Node::Base> expr) {
    last_ = move(expr);
}
void Calculator::save(const string& name) {
    vars_[name] = last_;
}
shared_ptr<Node::Base> Calculator::derivative() {
    return last_ = last_->derivative();
}
shared_ptr<Node::Base> Calculator::derivative(const string& name) {
    return last_ = vars_.at(name)->derivative();
}
double Calculator::evaluate(double x) const {
    return last_->evaluate(x);
}
double Calculator::evaluate(const string& name, double x) const {
    return vars_.at(name)->evaluate(x);
}
shared_ptr<Node::Base> Calculator::get() {
    return last_;
}
shared_ptr<Node::Base> Calculator::get(const string& name) {
    return last_ = vars_.at(name);
}
bool Calculator::var_exists(const string& name) const {
    return vars_.find(name) != vars_.end();
}

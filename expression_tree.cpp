#include "expression_tree.h"
#include "binary_operation.h"

#include <cmath>
#include <memory>
#include <iostream>
using namespace std;

namespace Node {
    bool Base::braces_needed_left(const ::BinaryOp::Base& op) const {
        return false;
    }
    bool Base::braces_needed_right(const ::BinaryOp::Base& op) const {
        return false;
    }
    optional<double> Base::get_const_value() const {
        return nullopt;
    }
    bool Base::is_simplified() const {
        return is_simplified_;
    }
    
    Constant::Constant(double val) : val_(val) {
        is_simplified_ = true;
    }
    double Constant::evaluate(double x) const {
        return val_;
    }
    Ptr Constant::derivative() const {
        return make_unique<Constant>(0);
    }
    Ptr Constant::shallow_copy() {
        return make_unique<Constant>(val_);
    }
    Ptr Constant::deep_copy() const {
        return make_unique<Constant>(val_);
    }
    void Constant::print(ostream &out) const {
        out << val_;
    }
    optional<double> Constant::get_const_value() const {
        return val_;
    }
    Ptr Constant::make_simplified() {
        return shallow_copy();
    }
    
    Variable::Variable() {
        is_simplified_ = true;
    }
    double Variable::evaluate(double x) const {
        return x;
    }
    Ptr Variable::derivative() const {
        return make_unique<Constant>(1);
    }
    Ptr Variable::shallow_copy() {
        return make_unique<Variable>();
    }
    Ptr Variable::deep_copy() const {
        return make_unique<Variable>();
    }
    void Variable::print(ostream &out) const {
        out << 'x';
    }
    Ptr Variable::make_simplified() {
        return shallow_copy();
    }
    
    namespace BinaryOp {
        Base::Base(Ptr left, Ptr right, unique_ptr<::BinaryOp::Base> op)
        : left_(move(left)), right_(move(right)), op_(move(op)) {}
        
        bool Base::braces_needed_left(const ::BinaryOp::Base& op) const {
            return op_->get_priority() < op.get_priority()
            || (op_->get_priority() == op.get_priority()
                && !op.is_left_assoc());
        }
        bool Base::braces_needed_right(const ::BinaryOp::Base& op) const {
            return op_->get_priority() < op.get_priority()
            || (op_->get_priority() == op.get_priority()
                && op.is_left_assoc());
        }
        void Base::print(ostream& out) const {
            if (left_->braces_needed_left(*op_)) {
                out << '(';
                left_->print(out);
                out << ')';
            } else {
                left_->print(out);
            }
            out << ' ' << op_->repr() << ' ';
            if (right_->braces_needed_right(*op_)) {
                out << '(';
                right_->print(out);
                out << ')';
            } else {
                right_->print(out);
            }
        }
        void Base::simplify_children() {
            if (!left_->is_simplified()) {
                left_ = left_->make_simplified();
            }
            if (!right_->is_simplified()) {
                right_ = right_->make_simplified();
            }
        }
        unique_ptr<Constant> Base::try_make_constant() {
            assert(left_->is_simplified() && right_->is_simplified());
            if (!left_->get_const_value().has_value()
                || !right_->get_const_value().has_value()) {
                return nullptr;
            }
            return make_unique<Constant>(evaluate(0));
        }
        
        Sum::Sum(Ptr left, Ptr right)
        : CopyableBase_(move(left), move(right),
                        make_unique<::BinaryOp::Sum>()) {}
        double Sum::evaluate(double x) const {
            return left_->evaluate(x) + right_->evaluate(x);
        }
        Ptr Sum::derivative() const {
            return ::make_simplified<Sum>(
                left_->derivative(),
                right_->derivative()
            );
        }
        Ptr Sum::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                simplify_children();
                if (auto ptr = try_make_constant()) {
                    return ptr;
                }
                if (left_->get_const_value().has_value()) {
                    swap(left_, right_);
                }
                if (auto right_val = right_->get_const_value();
                    right_val.has_value() && abs(*right_val) <= EPS) {
                    return move(left_);
                }
            }
            return shallow_copy();
        }
        
        Diff::Diff(Ptr left, Ptr right)
        : CopyableBase_(move(left), move(right),
                        make_unique<::BinaryOp::Diff>()) {}
        double Diff::evaluate(double x) const {
            return left_->evaluate(x) - right_->evaluate(x);
        }
        Ptr Diff::derivative() const {
            return ::make_simplified<Diff>(
                left_->derivative(),
                right_->derivative()
             );
        }
        Ptr Diff::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                simplify_children();
                if (auto ptr = try_make_constant()) {
                    return ptr;
                }
                if (auto left_val = left_->get_const_value();
                    left_val.has_value() && abs(*left_val) <= EPS) {
                    return ::make_simplified<UnaryFunc::Neg>(move(right_));
                }
                if (auto right_val = right_->get_const_value();
                    right_val.has_value() && abs(*right_val) <= EPS) {
                    return move(left_);
                }
            }
            return shallow_copy();
        }
        
        Mult::Mult(Ptr left, Ptr right)
        : CopyableBase_(move(left), move(right),
                        make_unique<::BinaryOp::Mult>()) {}
        double Mult::evaluate(double x) const {
            return left_->evaluate(x) * right_->evaluate(x);
        }
        Ptr Mult::derivative() const {
            return ::make_simplified<Sum>(
                ::make_simplified<Mult>(
                    left_->derivative(),
                    right_->deep_copy()
                ),
                ::make_simplified<Mult>(
                    left_->deep_copy(),
                    right_->derivative()
                )
            );
        }
        Ptr Mult::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                simplify_children();
                if (auto ptr = try_make_constant()) {
                    return ptr;
                }
                if (right_->get_const_value().has_value()) {
                    swap(left_, right_);
                }
                if (auto left_val = left_->get_const_value();
                    left_val.has_value()) {
                    if (abs(*left_val) <= EPS) {
                        return make_unique<Constant>(0);
                    }
                    if (abs(*left_val - 1) <= EPS) {
                        return move(right_);
                    }
                }
            }
            return shallow_copy();
        }
        
        Div::Div(Ptr left, Ptr right)
        : CopyableBase_(move(left), move(right),
                        make_unique<::BinaryOp::Div>()) {}
        double Div::evaluate(double x) const {
            return left_->evaluate(x) / right_->evaluate(x);
        }
        Ptr Div::derivative() const {
            return ::make_simplified<Div>(
                ::make_simplified<Diff>(
                    ::make_simplified<Mult>(
                        left_->derivative(),
                        right_->deep_copy()
                    ),
                    ::make_simplified<Mult>(
                        left_->deep_copy(),
                        right_->derivative()
                    )
                ),
                ::make_simplified<Pow>(
                    right_->deep_copy(),
                    make_unique<Constant>(2)
                )
            );
        }
        Ptr Div::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                simplify_children();
                if (auto ptr = try_make_constant()) {
                    return ptr;
                }
                if (auto left_val = left_->get_const_value();
                    left_val.has_value() && abs(*left_val) <= EPS) {
                    return make_unique<Constant>(0);
                }
                if (auto right_val = right_->get_const_value();
                    right_val.has_value() && abs(*right_val - 1) <= EPS) {
                    return move(left_);
                }
            }
            return shallow_copy();
        }
        
        Pow::Pow(Ptr left, Ptr right)
        : CopyableBase_(move(left), move(right),
                        make_unique<::BinaryOp::Pow>()) {}
        double Pow::evaluate(double x) const {
            return pow(left_->evaluate(x), right_->evaluate(x));
        }
        Ptr Pow::derivative() const {
            if (auto power = right_->get_const_value(); power.has_value()) {
                return ::make_simplified<Mult>(
                    make_unique<Constant>(*power),
                    ::make_simplified<Pow>(
                        left_->deep_copy(),
                        make_unique<Constant>(*power - 1)
                    )
                );
            }
            return ::make_simplified<Mult>(
                deep_copy(),
                ::make_simplified<Sum>(
                    ::make_simplified<Div>(
                        ::make_simplified<Mult>(
                            left_->derivative(),
                            right_->deep_copy()
                        ),
                        left_->deep_copy()
                    ),
                    ::make_simplified<Mult>(
                        ::make_simplified<UnaryFunc::Ln>(
                            left_->deep_copy()
                        ),
                        right_->derivative()
                    )
                )
            );
        }
        Ptr Pow::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                simplify_children();
                if (auto ptr = try_make_constant()) {
                    return ptr;
                }
                if (auto left_val = left_->get_const_value();
                    left_val.has_value()) {
                    if (abs(*left_val) <= EPS) {
                        return make_unique<Constant>(0);
                    }
                    if (abs(*left_val - 1) <= EPS) {
                        return make_unique<Constant>(1);
                    }
                }
                if (auto right_val = right_->get_const_value();
                    right_val.has_value()) {
                    if (abs(*right_val) <= EPS) {
                        return make_unique<Constant>(1);
                    }
                    if (abs(*right_val - 1) <= EPS) {
                        return move(left_);
                    }
                }
            }
            return shallow_copy();
        }
    }
    
    namespace UnaryFunc {
        Base::Base(Ptr child) : child_(move(child)) {}
        Ptr Base::make_simplified() {
            if (!is_simplified_) {
                is_simplified_ = true;
                if (!child_->is_simplified()) {
                    child_ = child_->make_simplified();
                }
                if (child_->get_const_value().has_value()) {
                    return make_unique<Constant>(evaluate(0));
                }
            }
            return shallow_copy();
        }
        
        double Sin::evaluate(double x) const {
            return sin(child_->evaluate(x));
        }
        void Sin::print(std::ostream &out) const {
            out << "sin(";
            child_->print(out);
            out << ")";
        }
        Ptr Sin::derivative() const {
            return ::make_simplified<BinaryOp::Mult>(
                ::make_simplified<Cos>(child_->deep_copy()),
                child_->derivative()
           );
        }
        
        double Cos::evaluate(double x) const {
            return cos(child_->evaluate(x));
        }
        void Cos::print(std::ostream &out) const {
            out << "cos(";
            child_->print(out);
            out << ")";
        }
        Ptr Cos::derivative() const {
            return ::make_simplified<BinaryOp::Mult>(
                ::make_simplified<Neg>(::make_simplified<Sin>(child_->deep_copy())),
                child_->derivative()
            );
        }
        
        double Tan::evaluate(double x) const {
            return tan(child_->evaluate(x));
        }
        void Tan::print(std::ostream &out) const {
            out << "tan(";
            child_->print(out);
            out << ")";
        }
        Ptr Tan::derivative() const {
            return ::make_simplified<BinaryOp::Mult>(
                ::make_simplified<BinaryOp::Div>(
                    make_unique<Constant>(1),
                    ::make_simplified<BinaryOp::Pow>(
                        ::make_simplified<Cos>(child_->deep_copy()),
                        make_unique<Constant>(2)
                    )
                ),
                child_->derivative()
            );
        }
        
        double Cot::evaluate(double x) const {
            return 1 / tan(child_->evaluate(x));
        }
        void Cot::print(std::ostream &out) const {
            out << "cot(";
            child_->print(out);
            out << ")";
        }
        Ptr Cot::derivative() const {
            return ::make_simplified<BinaryOp::Mult>(
                ::make_simplified<Neg>(::make_simplified<BinaryOp::Div>(
                    make_unique<Constant>(1),
                    ::make_simplified<BinaryOp::Pow>(
                        ::make_simplified<Sin>(child_->deep_copy()),
                        make_unique<Constant>(2)
                    )
                )),
                child_->derivative()
            );
        }
        
        double Neg::evaluate(double x) const {
            return -child_->evaluate(x);
        }
        void Neg::print(std::ostream &out) const {
            out << "-(";
            child_->print(out);
            out << ")";
        }
        Ptr Neg::derivative() const {
            return ::make_simplified<Neg>(child_->deep_copy());
        }
        bool Neg::braces_needed_left(const ::BinaryOp::Base& op) const {
            return op.get_type() == ::BinaryOp::Type::POW;
        }
        
        bool Neg::braces_needed_right(const ::BinaryOp::Base& op) const {
            return true;
        }
        
        double Ln::evaluate(double x) const {
            return log(child_->evaluate(x));
        }
        void Ln::print(std::ostream &out) const {
            out << "ln(";
            child_->print(out);
            out << ")";
        }
        Ptr Ln::derivative() const {
            return ::make_simplified<BinaryOp::Mult>(
                ::make_simplified<BinaryOp::Div>(
                   make_unique<Constant>(1),
                   child_->deep_copy()
                ),
                child_->derivative()
            );
        }
    }
}

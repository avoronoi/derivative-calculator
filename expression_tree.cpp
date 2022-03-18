#include "expression_tree.h"
#include "binary_operation.h"

#include <cmath>
#include <memory>

using namespace std;

namespace Node {
    bool Base::braces_needed_left(const ::BinaryOp::Base& op) const {
        return false;
    }
    bool Base::braces_needed_right(const ::BinaryOp::Base& op) const {
        return false;
    }
    
    Constant::Constant(double val) : val_(val) {}
    double Constant::evaluate(double x) const {
        return val_;
    }
    Ptr Constant::derivative() const {
        return make_unique<Constant>(0);
    }
    Ptr Constant::clone_ptr() const {
        return make_unique<Constant>(val_);
    }
    void Constant::print(ostream &out) const {
        out << val_;
    }
    
    double Variable::evaluate(double x) const {
        return x;
    }
    Ptr Variable::derivative() const {
        return make_unique<Constant>(1);
    }
    Ptr Variable::clone_ptr() const {
        return make_unique<Variable>();
    }
    void Variable::print(ostream &out) const {
        out << 'x';
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
        
        Sum::Sum(Ptr left, Ptr right)
        : CloneableBase_(move(left), move(right),
                         make_unique<::BinaryOp::Sum>()) {}
        double Sum::evaluate(double x) const {
            return left_->evaluate(x) + right_->evaluate(x);
        }
        Ptr Sum::derivative() const {
            return make_unique<Sum>(
                left_->derivative(),
                right_->derivative()
            );
        }
        
        Diff::Diff(Ptr left, Ptr right)
        : CloneableBase_(move(left), move(right),
                         make_unique<::BinaryOp::Diff>()) {}
        double Diff::evaluate(double x) const {
            return left_->evaluate(x) - right_->evaluate(x);
        }
        Ptr Diff::derivative() const {
            return make_unique<Diff>(
                left_->derivative(),
                right_->derivative()
             );
        }
        
        Mult::Mult(Ptr left, Ptr right)
        : CloneableBase_(move(left), move(right),
                         make_unique<::BinaryOp::Mult>()) {}
        double Mult::evaluate(double x) const {
            return left_->evaluate(x) * right_->evaluate(x);
        }
        Ptr Mult::derivative() const {
            return make_unique<Sum>(
                make_unique<Mult>(
                    left_->derivative(),
                    right_->clone_ptr()
                ),
                make_unique<Mult>(
                    left_->clone_ptr(),
                    right_->derivative()
                )
            );
        }
        
        Div::Div(Ptr left, Ptr right)
        : CloneableBase_(move(left), move(right),
                         make_unique<::BinaryOp::Div>()) {}
        double Div::evaluate(double x) const {
            return left_->evaluate(x) / right_->evaluate(x);
        }
        Ptr Div::derivative() const {
            return make_unique<Div>(
                make_unique<Diff>(
                    make_unique<Mult>(
                        left_->derivative(),
                        right_->clone_ptr()
                    ),
                    make_unique<Mult>(
                        left_->clone_ptr(),
                        right_->derivative()
                    )
                ),
                make_unique<Pow>(
                    right_->clone_ptr(),
                    make_unique<Constant>(2)
                )
            );
        }
        
        Pow::Pow(Ptr left, Ptr right)
        : CloneableBase_(move(left), move(right),
                         make_unique<::BinaryOp::Pow>()) {}
        double Pow::evaluate(double x) const {
            return pow(left_->evaluate(x), right_->evaluate(x));
        }
        Ptr Pow::derivative() const {
            return make_unique<Mult>(
                clone_ptr(),
                make_unique<Sum>(
                    make_unique<Div>(
                        make_unique<Mult>(
                            left_->derivative(),
                            right_->clone_ptr()
                        ),
                        left_->clone_ptr()
                    ),
                    make_unique<Mult>(
                        make_unique<UnaryFunc::Ln>(
                            left_->clone_ptr()
                        ),
                        right_->derivative()
                    )
                )
            );
        }
    }
    
    namespace UnaryFunc {
        Base::Base(Ptr child) : child_(move(child)) {}
        
        double Sin::evaluate(double x) const {
            return sin(child_->evaluate(x));
        }
        void Sin::print(std::ostream &out) const {
            out << "sin(";
            child_->print(out);
            out << ")";
        }
        Ptr Sin::derivative() const {
            return make_unique<BinaryOp::Mult>(
                make_unique<Cos>(child_->clone_ptr()),
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
            return make_unique<BinaryOp::Mult>(
                make_unique<Neg>(make_unique<Sin>(child_->clone_ptr())),
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
            return make_unique<BinaryOp::Mult>(
                make_unique<BinaryOp::Div>(
                    make_unique<Constant>(1),
                    make_unique<BinaryOp::Pow>(
                        make_unique<Cos>(child_->clone_ptr()),
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
            return make_unique<BinaryOp::Mult>(
                make_unique<Neg>(make_unique<BinaryOp::Div>(
                    make_unique<Constant>(1),
                    make_unique<BinaryOp::Pow>(
                        make_unique<Sin>(child_->clone_ptr()),
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
            return make_unique<Neg>(child_->clone_ptr());
        }
        bool Neg::braces_needed_left(const ::BinaryOp::Base& op) const {
            return true;
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
            return make_unique<BinaryOp::Mult>(
                make_unique<BinaryOp::Div>(
                   make_unique<Constant>(1),
                   child_->clone_ptr()
                ),
                child_->derivative()
            );
        }
    }
}

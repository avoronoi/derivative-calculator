#include "binary_operation.h"

namespace BinaryOp {
    Ptr& Ptr::operator=(Ptr&& other) {
        return static_cast<Ptr&>(
            BasePtr_::operator=(std::move(static_cast<BasePtr_&>(other)))
        );
    }
    Ptr& Ptr::operator=(const Ptr& other) {
        return static_cast<Ptr&>(
            BasePtr_::operator=(other->clone_ptr())
        );
    }
    Ptr::Ptr(Ptr&& other) {
        *this = move(other);
    }
    Ptr::Ptr(const Ptr& other) {
        *this = other;
    }
    
    bool Base::is_left_assoc() const {
        return true;
    }
    
    int Sum::get_priority() const {
        return 1;
    }
    Type Sum::get_type() const {
        return Type::SUM;
    }
    char Sum::repr() const {
        return '+';
    }
    
    int Diff::get_priority() const {
        return 1;
    }
    Type Diff::get_type() const {
        return Type::DIFF;
    }
    char Diff::repr() const {
        return '-';
    }
    
    int Mult::get_priority() const {
        return 2;
    }
    Type Mult::get_type() const {
        return Type::MULT;
    }
    char Mult::repr() const {
        return '*';
    }
    
    int Div::get_priority() const {
        return 2;
    }
    Type Div::get_type() const {
        return Type::DIV;
    }
    char Div::repr() const {
        return '/';
    }
    
    int Pow::get_priority() const {
        return 3;
    }
    bool Pow::is_left_assoc() const {
        return false;
    }
    Type Pow::get_type() const {
        return Type::POW;
    }
    char Pow::repr() const {
        return '^';
    }
}
std::ostream& operator<<(std::ostream& out, const BinaryOp::Base& op) {
    return out << op.repr();
}

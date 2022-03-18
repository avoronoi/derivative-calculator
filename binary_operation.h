#pragma once

#include <ostream>
#include <memory>

namespace BinaryOp {
    enum Type {
        SUM, DIFF, MULT, DIV, POW
    };

    struct Base {
        virtual int get_priority() const = 0;
        virtual bool is_left_assoc() const;
        virtual Type get_type() const = 0;
        virtual char repr() const = 0;
//        virtual constructor idiom
        virtual std::unique_ptr<Base> clone_ptr() const = 0;
        
        virtual ~Base() = default;
    };
    
    using BasePtr_ = std::unique_ptr<Base>;
//    extend class so that it can have copy constructor/assignment operator; we need it so that we can copy Token objects
    class Ptr : public BasePtr_ {
    public:
        using BasePtr_::BasePtr_;
        
        Ptr(Ptr&& other);
        Ptr(const Ptr& other);
        
        Ptr& operator=(Ptr&& other);
        Ptr& operator=(const Ptr& other);
    };
    
    template<typename T>
    struct CloneableBase_ : public Base {
    public:
        BasePtr_ clone_ptr() const final {
            return std::make_unique<T>();
        }
    };

    struct Sum : public CloneableBase_<Sum> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Diff : public CloneableBase_<Diff> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Mult : public CloneableBase_<Mult> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Div : public CloneableBase_<Div> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Pow : public CloneableBase_<Pow> {
        int get_priority() const final;
        bool is_left_assoc() const final;
        Type get_type() const final;
        char repr() const final;
    };
}
std::ostream& operator<<(std::ostream& out, const BinaryOp::Base& op);

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
        virtual std::unique_ptr<Base> deep_copy() const = 0;
        
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
    struct CopyableBase_ : public Base {
    public:
        BasePtr_ deep_copy() const final {
            return std::make_unique<T>();
        }
    };

    struct Sum : public CopyableBase_<Sum> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Diff : public CopyableBase_<Diff> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Mult : public CopyableBase_<Mult> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Div : public CopyableBase_<Div> {
        int get_priority() const final;
        Type get_type() const final;
        char repr() const final;
    };

    struct Pow : public CopyableBase_<Pow> {
        int get_priority() const final;
        bool is_left_assoc() const final;
        Type get_type() const final;
        char repr() const final;
    };
}
std::ostream& operator<<(std::ostream& out, const BinaryOp::Base& op);

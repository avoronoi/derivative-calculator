#pragma once

#include "binary_operation.h"

#include <memory>
#include <optional>

namespace Node {
    class Base;
    using Ptr = std::unique_ptr<Base>;
    
    class Base {
    public:
        virtual double evaluate(double x) const = 0;
        virtual Ptr derivative() const = 0;
        virtual Ptr shallow_copy() = 0;
        virtual Ptr deep_copy() const = 0;
        virtual void print(std::ostream& out) const = 0;
        
//        to put braces only where it is needed
        virtual bool braces_needed_left(const ::BinaryOp::Base& op) const;
        virtual bool braces_needed_right(const ::BinaryOp::Base& op) const;
        
        virtual std::optional<double> get_const_value() const;
//        modifies the object, so the result needs to be assigned to this object
        virtual Ptr make_simplified() = 0;
        virtual bool is_simplified() const;
        
        virtual ~Base() = default;
    protected:
        bool is_simplified_ = false;
        static constexpr double EPS = 1e-10;
    };
    
    class Constant : public Base {
    public:
        Constant(double val);
        double evaluate(double x) const final;
        Ptr derivative() const final;
        Ptr shallow_copy() final;
        Ptr deep_copy() const final;
        void print(std::ostream& out) const final;
        std::optional<double> get_const_value() const final;
        Ptr make_simplified() final;
    private:
        const double val_;
    };
    
    class Variable : public Base {
    public:
        Variable();
        double evaluate(double x) const final;
        Ptr derivative() const final;
        Ptr shallow_copy() final;
        Ptr deep_copy() const final;
        void print(std::ostream& out) const final;
        Ptr make_simplified() final;
    };
    
    namespace BinaryOp {
        class Base : public Node::Base {
        public:
            Base(Ptr left, Ptr right, std::unique_ptr<::BinaryOp::Base> op);
            void print(std::ostream& out) const final;
            bool braces_needed_left(const ::BinaryOp::Base& op) const final;
            bool braces_needed_right(const ::BinaryOp::Base& op) const final;
        protected:
            Ptr left_, right_;
            void simplify_children();
            std::unique_ptr<Constant> try_make_constant();
        private:
            std::unique_ptr<::BinaryOp::Base> op_;
        };
        
        template<typename T>
        class CopyableBase_ : public Base {
        public:
            using Base::Base;
            Ptr shallow_copy() final {
                auto ret = std::make_unique<T>(std::move(left_),
                                               std::move(right_));
                ret->is_simplified_ = is_simplified_;
                return ret;
            }
            Ptr deep_copy() const final {
                auto ret = std::make_unique<T>(left_->deep_copy(),
                                               right_->deep_copy());
                ret->is_simplified_ = is_simplified_;
                return ret;
            }
        };
        
        class Sum : public CopyableBase_<Sum> {
        public:
            Sum(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
            Ptr make_simplified() final;
        };
        
        class Diff : public CopyableBase_<Diff> {
        public:
            Diff(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
            Ptr make_simplified() final;
        };
        
        class Mult : public CopyableBase_<Mult> {
        public:
            Mult(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
            Ptr make_simplified() final;
        };
        
        class Div : public CopyableBase_<Div> {
        public:
            Div(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
            Ptr make_simplified() final;
        };
        
        class Pow : public CopyableBase_<Pow> {
        public:
            Pow(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
            Ptr make_simplified() final;
        };
    }
    
    namespace UnaryFunc {
        class Base : public Node::Base {
        public:
            Base(Ptr child);
            Ptr make_simplified() final;
        protected:
            Ptr child_;
        };
        
        template<typename T>
        class CopyableBase_ : public Base {
        public:
            using Base::Base;
            Ptr shallow_copy() final {
                auto ret = std::make_unique<T>(std::move(child_));
                ret->is_simplified_ = is_simplified_;
                return ret;
            }
            Ptr deep_copy() const final {
                auto ret = std::make_unique<T>(child_->deep_copy());
                ret->is_simplified_ = is_simplified_;
                return ret;
            }
        };
        
        class Sin : public CopyableBase_<Sin> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Cos : public CopyableBase_<Cos> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Tan : public CopyableBase_<Tan> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Cot : public CopyableBase_<Cot> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Neg : public CopyableBase_<Neg> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
            bool braces_needed_left(const ::BinaryOp::Base& op) const final;
            bool braces_needed_right(const ::BinaryOp::Base& op) const final;
        };
        
        class Ln : public CopyableBase_<Ln> {
        public:
            using CopyableBase_::CopyableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
    }
}

template<typename T, typename... Args>
Node::Ptr make_simplified(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...)
        ->make_simplified();
}

#pragma once

#include "binary_operation.h"

#include <memory>

namespace Node {
    class Base {
    public:
        virtual double evaluate(double x) const = 0;
        virtual std::unique_ptr<Base> derivative() const = 0;
        virtual std::unique_ptr<Base> clone_ptr() const = 0;
        virtual void print(std::ostream& out) const = 0;
        
//        to put braces only where it is needed
        virtual bool braces_needed_left(const ::BinaryOp::Base& op) const;
        virtual bool braces_needed_right(const ::BinaryOp::Base& op) const;
        
        virtual ~Base() = default;
    };
    
    using Ptr = std::unique_ptr<Base>;
    
    class Constant : public Base {
    public:
        Constant(double val);
        double evaluate(double x) const final;
        Ptr derivative() const final;
        Ptr clone_ptr() const final;
        void print(std::ostream& out) const final;
    private:
        const double val_;
    };
    
    class Variable : public Base {
    public:
        double evaluate(double x) const final;
        Ptr derivative() const final;
        Ptr clone_ptr() const final;
        void print(std::ostream& out) const final;
    };
    
    namespace BinaryOp {
        class Base : public Node::Base {
        public:
            Base(Ptr left, Ptr right, std::unique_ptr<::BinaryOp::Base> op);
            void print(std::ostream& out) const final;
            bool braces_needed_left(const ::BinaryOp::Base& op) const final;
            bool braces_needed_right(const ::BinaryOp::Base& op) const final;
        protected:
            const Ptr left_, right_;
        private:
            std::unique_ptr<::BinaryOp::Base> op_;
        };
        
        template<typename T>
        class CloneableBase_ : public Base {
        public:
            using Base::Base;
            Ptr clone_ptr() const final {
                return std::make_unique<T>(left_->clone_ptr(), right_->clone_ptr());
            }
        };
        
        class Sum : public CloneableBase_<Sum> {
        public:
            Sum(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
        };
        
        class Diff : public CloneableBase_<Diff> {
        public:
            Diff(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
        };
        
        class Mult : public CloneableBase_<Mult> {
        public:
            Mult(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
        };
        
        class Div : public CloneableBase_<Div> {
        public:
            Div(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
        };
        
        class Pow : public CloneableBase_<Pow> {
        public:
            Pow(Ptr left, Ptr right);
            double evaluate(double x) const final;
            Ptr derivative() const final;
        };
    }
    
    namespace UnaryFunc {
        class Base : public Node::Base {
        public:
            Base(Ptr child);
        protected:
            const Ptr child_;
        };
        
        template<typename T>
        class CloneableBase_ : public Base {
        public:
            using Base::Base;
            Ptr clone_ptr() const final {
                return std::make_unique<T>(child_->clone_ptr());
            }
        };
        
        class Sin : public CloneableBase_<Sin> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Cos : public CloneableBase_<Cos> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Tan : public CloneableBase_<Tan> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Cot : public CloneableBase_<Cot> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
        
        class Neg : public CloneableBase_<Neg> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
            bool braces_needed_left(const ::BinaryOp::Base& op) const final;
            bool braces_needed_right(const ::BinaryOp::Base& op) const final;
        };
        
        class Ln : public CloneableBase_<Ln> {
        public:
            using CloneableBase_::CloneableBase_;
            double evaluate(double x) const final;
            Ptr derivative() const final;
            void print(std::ostream& out) const final;
        };
    }
}

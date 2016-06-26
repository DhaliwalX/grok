#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "parser/common.h"
#include "lexer/lexer.h"
#include "vm/instruction-list.h"
#include "vm/instruction-builder.h"
#include <vector>
#include <string>
#include <map>

namespace grok { namespace parser {

class Expression {
public:
    virtual ~Expression() { }
    virtual std::ostream &operator<<(std::ostream &os) const = 0;
    virtual void emit(std::shared_ptr<grok::vm::InstructionBuilder>) = 0;
};

class NullLiteral : public Expression {
public:
    NullLiteral() { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
};

class IntegralLiteral : public Expression {
private:
    double value_;

public:
    IntegralLiteral(double value) : value_(value) { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
};

class StringLiteral : public Expression {
private:
    std::string str_;
public:
    StringLiteral(const std::string &str) : str_(str) { }
    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
};

class ArrayLiteral : public Expression {
public:
    ArrayLiteral(std::vector<std::unique_ptr<Expression>> exprs)
        : exprs_{ std::move(exprs) }
    { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
private:
    std::vector<std::unique_ptr<Expression>> exprs_;
};

class ObjectLiteral : public Expression {
public:
    ObjectLiteral(std::map<std::string, std::unique_ptr<Expression>> props)
        : Props{ std::move(props) }
    { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
private:
    std::map<std::string, std::unique_ptr<Expression>> Props;
};

class Identifier : public Expression {
private:
    std::string name_;
public:
    Identifier(const std::string &name) : name_(name) { }
    std::ostream &operator<<(std::ostream &os) const override;
    const std::string &GetName() const { return name_; }
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
};

class BooleanLiteral : public Expression {
    bool pred_;
public:
    BooleanLiteral(bool val) : pred_(val) { }   
    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
};

class FunctionCallExpression : public Expression {
public:
    FunctionCallExpression(std::string name,
        std::vector<std::unique_ptr<Expression>> args)
        : args_{ std::move(args) }, name_{ std::move(name) }
    { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
private:
    std::vector<std::unique_ptr<Expression>> args_;
    std::string name_;
};

class BinaryExpression : public Expression {
public:
    using Operator = TokenType;
    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;

private:
    Operator op_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
public:
    BinaryExpression(Operator op, std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }
};

class AssignExpression : public Expression {
public:
    AssignExpression(std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs)
        : lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }
    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;

private:
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class TernaryExpression : public Expression {
public:
    TernaryExpression(std::unique_ptr<Expression> first,
                      std::unique_ptr<Expression> second,
                      std::unique_ptr<Expression> third)
    : first_(std::move(first)), second_(std::move(second)),
        third_(std::move(third))
    { }
    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;

private:
    std::unique_ptr<Expression> first_;
    std::unique_ptr<Expression> second_;
    std::unique_ptr<Expression> third_;
};

class CommaExpression : public Expression {
public:
    CommaExpression(std::vector<std::unique_ptr<Expression>> exprs)
    : exprs_{ std::move(exprs) }
    { }
    std::ostream &operator<<(std::ostream &os) const override {
        return os;
    }
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;

private:
    std::vector<std::unique_ptr<Expression>> exprs_;
};

} // namespace parser
} // namespace grok

#endif  /* Expression.h */

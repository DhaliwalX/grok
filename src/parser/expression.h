#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "parser/common.h"
#include "lexer/lexer.h"
#include <vector>
#include <string>

namespace grok { namespace parser {

class Expression {
public:
    virtual ~Expression() { }
    virtual std::ostream &operator<<(std::ostream &os) const = 0;
};

class IntegralLiteral : public Expression {
private:
    double value_;

public:
    IntegralLiteral(double value) : value_(value) { }

    std::ostream &operator<<(std::ostream &os) const override;
};

class StringLiteral : public Expression {
private:
    std::string str_;
public:
    StringLiteral(const std::string &str) : str_(str) { }
    std::ostream &operator<<(std::ostream &os) const override;
};

class Identifier : public Expression {
private:
    std::string name_;
public:
    Identifier(const std::string &name) : name_(name) { }
    std::ostream &operator<<(std::ostream &os) const override;
};

class BooleanLiteral : public Expression {
    bool pred_;
public:
    BooleanLiteral(bool val) : pred_(val) { }   
    std::ostream &operator<<(std::ostream &os) const override;
};

class BinaryExpression : public Expression {
public:
    using Operator = TokenType;
    std::ostream &operator<<(std::ostream &os) const override;

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

private:
    std::unique_ptr<Expression> first_;
    std::unique_ptr<Expression> second_;
    std::unique_ptr<Expression> third_;
};

class CommaExpression : public Expression {
public:
    CommaExpression(std::vector<std::unique_ptr<Expression>> exprs)
    : exprs_(std::move(exprs))
    { }
    std::ostream &operator<<(std::ostream &os) const override {
        return os;
    }

private:
    std::vector<std::unique_ptr<Expression>> exprs_;
};

} // namespace parser
} // namespace grok

#endif  /* Expression.h */

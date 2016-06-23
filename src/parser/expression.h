#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "parser/common.h"
#include "lexer/lexer.h"

namespace grok { namespace parser {

class Expression {
public:
    virtual ~Expression();
};

class IntegralLiteral : public Expression {
private:
    double value_;

public:
    IntegralLiteral(double value) : value_(value) { }
};

class StringLiteral : public Expression {
private:
    std::string str_;
public:
    StringLiteral(const std::string &str) : str_(str) { }
};

class Identifier : public Expression {
private:
    std::string name_;
public:
    Identifier(const std::string &name) : name_(name) { }
};

class BooleanLiteral : public Expression {
    bool pred_;
public:
    BooleanLiteral(bool val) : pred_(val) { }   
};

class BinaryExpression : public Expression {
public:
    using Operator = TokenType;

private:
    Operator op_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
public:
    BinaryExpression(Operator op, std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }
};

} // namespace parser
} // namespace grok

#endif  /* Expression.h */

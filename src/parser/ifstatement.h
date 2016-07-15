#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include "parser/expression.h"

namespace grok {
namespace parser {

class IfStatement : public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    IfStatement(ExprPtr cond, ExprPtr body)
        : condition_{ std::move(cond) }, body_{ std::move(body) }
    { }

    DEFINE_NODE_TYPE(IfStatement);

    std::unique_ptr<Expression> &condition() { return condition_; }
    std::unique_ptr<Expression> &body() { return body_; }
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> body_;
};

class IfElseStatement : public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    IfElseStatement(ExprPtr cond, ExprPtr body, ExprPtr el)
    : condition_{ std::move(cond) },
      body_{ std::move(body) },
      else_{ std::move(el) }
    { }

    DEFINE_NODE_TYPE(IfElseStatement);

    std::unique_ptr<Expression> &condition() { return condition_; }
    std::unique_ptr<Expression> &body() { return body_; }
    std::unique_ptr<Expression> &els() { return else_; }
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> body_;
    std::unique_ptr<Expression> else_;
};

} // end parser
} // end grok

#endif

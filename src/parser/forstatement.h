#ifndef FOR_STATEMENT_H_
#define FOR_STATEMENT_H_

#include "parser/expression.h"

namespace grok {
namespace parser {

class ForStatement : public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    ForStatement(ExprPtr init, ExprPtr condition, ExprPtr update, ExprPtr body)
        : init_{ std::move(init) }, condition_{ std::move(condition) },
          update_{ std::move(update) }, body_{ std::move(body) }
    { }

    DEFINE_NODE_TYPE(ForStatement);

    ExprPtr &init() { return init_; }
    ExprPtr &condition() { return condition_; }
    ExprPtr &update() { return update_; }
    ExprPtr &body() { return body_; }

private:
    std::unique_ptr<Expression> init_;
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> update_;
    std::unique_ptr<Expression> body_;
};

class WhileStatement : public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    WhileStatement(ExprPtr condition, ExprPtr body)
        : condition_{ std::move(condition) },
          body_{ std::move(body) }
    { }

    DEFINE_NODE_TYPE(WhileStatement);

    ExprPtr &condition() { return condition_; }
    ExprPtr &body() { return body_; }

private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> body_;
};

class DoWhileStatement : public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    DoWhileStatement(ExprPtr condition, ExprPtr body)
        : condition_{ std::move(condition) },
          body_{ std::move(body) }
    { }
    DEFINE_NODE_TYPE(DoWhileStatement);

    ExprPtr &condition() { return condition_; }
    ExprPtr &body() { return body_; }
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> body_;
};

} // parser
} // grok

#endif // forstatment.h

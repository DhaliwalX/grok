#ifndef BLOCK_STATEMENT_H_
#define BLOCK_STATEMENT_H_

#include "parser/expression.h"
#include <vector>

namespace grok {
namespace parser {

///  BlockStatment ::= class representing block statments
class BlockStatement : public Expression {
public:
    BlockStatement(ExpressionList stmts)
        : stmts_{ std::move(stmts) }
    { }

    DEFINE_NODE_TYPE(BlockStatement);

    ExpressionList &statements() { return stmts_; }
    void PushExpression(std::unique_ptr<Expression> expr);
private:
    ExpressionList stmts_;
};

} // parser
} // grok

#endif // blockstatement.h

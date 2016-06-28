#include "parser/blockstatement.h"

using namespace grok::parser;

std::ostream &BlockStatement::operator<<(std::ostream &os) const
{
    os << "({";
    for (auto &stmt : stmts_) {
        *stmt << os;
    }
    return os << "})";
}

void BlockStatement::PushExpression(std::unique_ptr<Expression> expr)
{
    stmts_.push_back(std::move(expr));
}

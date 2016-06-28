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

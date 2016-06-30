#include "parser/forstatement.h"

using namespace grok::parser;
std::ostream &ForStatement::operator<<(std::ostream &os) const
{
    os << "(for (";
    *init_ << os;
    os << ";";
    *condition_ << os;
    os << ";";
    *update_ << os;
    os << ") {";
    *body_ << os;
    os << "})";
    return os;
}

std::ostream &WhileStatement::operator<<(std::ostream &os) const
{
    os << "(while (";
    *condition_ << os;
    os << ") {";
    *body_ << os;
    os << "})";
    return os;
}

std::ostream &DoWhileStatement::operator<<(std::ostream &os) const
{
    os << "(do {";
    *body_ << os;
    os << "}) while (";
    *condition_ << os;
    return os << ")";
}

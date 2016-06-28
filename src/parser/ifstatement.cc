#include "parser/ifstatement.h"

using namespace grok::parser;

std::ostream &IfStatement::operator<<(std::ostream &os) const
{
    os << "(if (";
    *condition_ << os << ") {";
    *body_ << os;
    os << "})";
    return os;
}

std::ostream &IfElseStatement::operator<<(std::ostream &os) const
{
    os << "(if (";
    *condition_ << os << ") {";
    *body_ << os;
    os << "} else {";
    *else_ << os;
    os << "})";
    return os;
}

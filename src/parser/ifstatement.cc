#include "parser/ifstatement.h"

using namespace grok::parser;

std::ostream &IfStatement::operator<<(std::ostream &os) const
{
    os << "if" << std::endl;
    *condition_ << os;
    *body_ << os;
    os << "endif" << std::endl;
    return os;
}

std::ostream &IfElseStatement::operator<<(std::ostream &os) const
{
    os << "if" << std::endl;
    *condition_ << os;
    *body_ << os;
    os << "else" << std::endl;
    *else_ << os;
    os << "endif";
    return os;
}

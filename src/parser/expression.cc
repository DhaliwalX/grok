#include "parser/expression.h"
namespace grok { namespace parser {
std::ostream &IntegralLiteral::operator<<(std::ostream &os) const
{
    os << value_ << std::endl;
    return os;
}

std::ostream &StringLiteral::operator<<(std::ostream &os) const
{
    os << "'" << str_ << "'" << std::endl;
    return os;
}

std::ostream &Identifier::operator<<(std::ostream &os) const
{
    os << name_ << std::endl;
    return os;
}

std::ostream &BooleanLiteral::operator<<(std::ostream &os) const
{
    os << std::boolalpha << pred_ << std::endl;
    return os;
}

std::ostream &BinaryExpression::operator<<(std::ostream &os) const
{
    os << token_type[op_] << std::endl;

    // weird!
    *lhs_ << os << std::endl;
    *rhs_ << os << std::endl;

    return os;
}

std::ostream &AssignExpression::operator<<(std::ostream &os) const
{
    os << '=' << std::endl;
    *lhs_ << os << std::endl;
    *rhs_ << os << std::endl;
    return os;
}

std::ostream &TernaryExpression::operator<<(std::ostream &os) const
{
    *first_ << os << std::endl;
    os << '?' << std::endl;
    *second_ << os << std::endl;
    os << ":" << std::endl;
    *third_ << os << std::endl;

    return os;
}

} // namespace parser
} // namespace grok
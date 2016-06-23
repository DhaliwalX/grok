#include "parser/forstatement.h"

using namespace grok::parser;
std::ostream &ForStatement::operator<<(std::ostream &os) const
{
    os << "for (\n";
    *init_ << os;
    os << ";\n";
    *condition_ << os;
    os << ";\n";
    *update_ << os;
    os << ") {\n";
    *body_ << os;
    os << "}\n";
    return os;
}

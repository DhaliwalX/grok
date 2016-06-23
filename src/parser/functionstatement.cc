#include "parser/functionstatement.h"

using namespace grok::parser;

std::ostream &FunctionPrototype::operator<<(std::ostream &os) const
{
    os << "function " << name_ << std::endl;

    for (auto &i : args_) {
        os << i << ", ";
    }
    os << std::endl;

    return os;
}

const std::string &FunctionPrototype::GetName() const
{
    return name_;
}

const std::vector<std::string> &FunctionPrototype::GetArgs() const
{
    return args_;
}

std::ostream &FunctionStatement::operator<<(std::ostream &os) const
{
    *proto_ << os;
    *body_ << os;
    return os;
}

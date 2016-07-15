#ifndef FUNCTION_STATEMENT_H_
#define FUNCTION_STATEMENT_H_

#include "parser/expression.h"
#include <vector>
#include <string>

namespace grok {
namespace parser {

// FunctionPrototype - captures the prototype of the function
// which includes the name of the function and 
class FunctionPrototype : public Expression {
public:
    FunctionPrototype(std::string name, std::vector<std::string> args)
        : name_{ name }, args_{ std::move(args) }
    { }
    DEFINE_NODE_TYPE(FunctionPrototype);
    const std::string &GetName() const;
    const std::vector<std::string> &GetArgs() const;
private:
    std::string name_;
    std::vector<std::string> args_;
};

// FunctionStatement - captures the function statement
class FunctionStatement : public Expression {
public:
    FunctionStatement(std::unique_ptr<FunctionPrototype> proto,
        std::unique_ptr<Expression> body)
        : proto_{ std::move(proto) }, body_{ std::move(body) }
    { }

    DEFINE_NODE_TYPE(FunctionStatement);

    std::unique_ptr<FunctionPrototype> &proto() { return proto_; }
    std::unique_ptr<Expression> &body() { return body_; }
private:
    std::unique_ptr<FunctionPrototype> proto_;
    std::unique_ptr<Expression> body_;
};

} // parser
} // grok

#endif // functionstatment.h

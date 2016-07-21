#ifndef RETURN_STATEMENT_H_
#define RETURN_STATEMENT_H_

#include "parser/expression.h"

namespace grok {
namespace parser {

class ReturnStatement : public Expression {
public:
    ReturnStatement(std::unique_ptr<Expression> expr)
        : expr_{ std::move(expr) }
    { }

    std::ostream &operator<<(std::ostream &os) const override
    {
        os << "(return ";
        return *expr_ << os << ")";
    }
#ifndef NO_EMIT_FUNCTION
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
#endif
    void Accept(ASTVisitor *visitor) override;
private:
    std::unique_ptr<Expression> expr_;
};

} // parser
} // grok
#endif

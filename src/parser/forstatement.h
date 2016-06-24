#ifndef FOR_STATEMENT_H_
#define FOR_STATEMENT_H_

#include "parser/expression.h"

namespace grok {
namespace parser {

class ForStatement :public Expression {
    using ExprPtr = std::unique_ptr<Expression>;
public:
    ForStatement(ExprPtr init, ExprPtr condition, ExprPtr update, ExprPtr body)
        : init_{ std::move(init) }, condition_{ std::move(condition) },
          update_{ std::move(update) }, body_{ std::move(body) }
    { }

    std::ostream &operator<<(std::ostream &os) const override;
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
private:
    std::unique_ptr<Expression> init_;
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> update_;
    std::unique_ptr<Expression> body_;
};

} // parser
} // grok

#endif // forstatment.h

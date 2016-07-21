#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "parser/common.h"
#include "lexer/lexer.h"
#include "vm/instruction-list.h"
#include "vm/instruction-builder.h"
#include <vector>
#include <string>
#include <map>

namespace grok { namespace parser {
#ifdef NO_EMIT_FUNCTION
#   define EMIT_FUNCTION
#else
#   define EMIT_FUNCTION \
    void emit(std::shared_ptr<grok::vm::InstructionBuilder>) override;
#endif

#define DEFINE_NODE_TYPE(type) \
    void Accept(ASTVisitor *visitor) override;   \
    EMIT_FUNCTION \
    std::ostream &operator<<(std::ostream &os) const override

#define AST_NODE_LIST(M)    \
    M(NullLiteral)          \
    M(ThisHolder)           \
    M(IntegralLiteral)      \
    M(StringLiteral)        \
    M(ArrayLiteral)         \
    M(ObjectLiteral)        \
    M(Identifier)           \
    M(BooleanLiteral)       \
    M(ArgumentList)         \
    M(FunctionCallExpression)   \
    M(CallExpression)       \
    M(DotMemberExpression)  \
    M(IndexMemberExpression)    \
    M(MemberExpression)     \
    M(NewExpression)        \
    M(PrefixExpression)     \
    M(PostfixExpression)    \
    M(BinaryExpression)     \
    M(AssignExpression)     \
    M(TernaryExpression)    \
    M(CommaExpression)      \
    M(Declaration)          \
    M(DeclarationList)      \
    M(IfStatement)          \
    M(IfElseStatement)      \
    M(ForStatement)         \
    M(WhileStatement)       \
    M(DoWhileStatement)     \
    M(BlockStatement)       \
    M(FunctionPrototype)    \
    M(FunctionStatement)    \
    M(ReturnStatement)      \

class ASTVisitor;

class Expression {
public:
    virtual ~Expression() { }
    virtual void Accept(ASTVisitor *visitor) = 0;
    virtual std::ostream &operator<<(std::ostream &os) const = 0;
#ifndef NO_EMIT_FUNCTION
    virtual void emit(std::shared_ptr<grok::vm::InstructionBuilder>) = 0;
#endif
    virtual bool ProduceRValue() { return true; }
};

using ProxyArray = std::vector<std::unique_ptr<Expression>>;
using ProxyObject = std::map<std::string, std::unique_ptr<Expression>>;
using ExpressionList = std::vector<std::unique_ptr<Expression>>;

class NullLiteral : public Expression {
public:
    NullLiteral() { }

    DEFINE_NODE_TYPE(NullLiteral);
};

class ThisHolder : public Expression {
public:
    ThisHolder() { }

    DEFINE_NODE_TYPE(ThisHolder);
    bool ProduceRValue() override { return false; }
};

class IntegralLiteral : public Expression {
private:
    double value_;

public:
    IntegralLiteral(double value) : value_(value) { }
    double value() { return value_; }
    DEFINE_NODE_TYPE(IntegralLiteral);
};

class StringLiteral : public Expression {
private:
    std::string str_;
public:
    StringLiteral(const std::string &str) : str_(str) { }

    std::string &string() { return str_; }
    DEFINE_NODE_TYPE(StringLiteral);
};

class ArrayLiteral : public Expression {
public:
    ArrayLiteral(ProxyArray exprs)
        : exprs_{ std::move(exprs) }
    { }

    ProxyArray &exprs() { return exprs_; }

    typename ProxyArray::size_type length() { return exprs_.size(); }

    DEFINE_NODE_TYPE(ArrayLiteral);
private:
    ProxyArray exprs_;
};

class ObjectLiteral : public Expression {
public:
    ObjectLiteral(ProxyObject props)
        : Props{ std::move(props) }
    { }

    ProxyObject &proxy() { return Props; }
    bool IsEmpty() { return Props.empty(); }
    ProxyObject::size_type GetPropertyCount() { return Props.size(); }

    DEFINE_NODE_TYPE(ObjectLiteral);
private:
    ProxyObject Props;
};

class Identifier : public Expression {
private:
    std::string name_;
public:
    Identifier(const std::string &name) : name_(name) { }
    const std::string &GetName() const { return name_; }
    bool ProduceRValue() override { return false; }
    DEFINE_NODE_TYPE(Identifier);
};

class BooleanLiteral : public Expression {
    bool pred_;
public:
    BooleanLiteral(bool val) : pred_(val) { }   

    bool pred() { return pred_; }
    DEFINE_NODE_TYPE(BooleanLiteral);
};

class ArgumentList : public Expression {
public:
    ArgumentList(ProxyArray args)
        : args_{ std::move(args) }
    { }

    ProxyArray &args() { return args_; }
    typename ProxyArray::size_type length() { return args().size(); }

    DEFINE_NODE_TYPE(ArgumentList);
private:
    ProxyArray args_;
};

class FunctionCallExpression : public Expression {
public:
    FunctionCallExpression(std::unique_ptr<Expression> func,
        std::vector<std::unique_ptr<Expression>> args)
        : args_{ std::move(args) }, func_{ std::move(func) }
    { }

    DEFINE_NODE_TYPE(FunctionCallExpression);
    ProxyArray &args() { return args_; }
    typename ProxyArray::size_type length() { return args().size(); }

    std::unique_ptr<Expression> &func() { return func_; }

    bool ProduceRValue() override { return false; }
private:
    ProxyArray args_;
    std::unique_ptr<Expression> func_;
};

class CallExpression : public Expression {
public:
    CallExpression(std::unique_ptr<Expression> func,
        ExpressionList members)
        : func_(std::move(func)), members_(std::move(members))
    { }
    DEFINE_NODE_TYPE(CallExpression);
    ExpressionList &members() { return members_; }
    typename ExpressionList::size_type length() { return members().size(); }

    std::unique_ptr<Expression> &func() { return func_; }
    bool ProduceRValue() override { return false; }
private:
    std::unique_ptr<Expression> func_;
    ExpressionList members_;
};

class DotMemberExpression : public Expression {
public:
    DotMemberExpression(std::unique_ptr<Identifier> mem)
        : mem_(std::move(mem))
    { }
    DEFINE_NODE_TYPE(DotMemberExpression);

    std::unique_ptr<Identifier> &member() { return mem_; }
    bool ProduceRValue() override { return false; }
private:
    std::unique_ptr<Identifier> mem_;
};

class IndexMemberExpression : public Expression {
public:
    IndexMemberExpression(std::unique_ptr<Expression> expr)
        : expr_{ std::move(expr) }
    { }

    DEFINE_NODE_TYPE(IndexMemberExpression);
    std::unique_ptr<Expression> &expr() { return expr_; }
    bool ProduceRValue() override { return false; }
private:
    std::unique_ptr<Expression> expr_;
};

class MemberExpression : public Expression {
public:
    MemberExpression(ExpressionList members)
        : members_{ std::move(members) }
    { }

    DEFINE_NODE_TYPE(MemberExpression);
    ExpressionList &members() { return members_; }
    bool ProduceRValue() override { return false; }
private:
    ExpressionList members_;
};

class NewExpression : public Expression {
public:
    NewExpression(std::unique_ptr<Expression> member)
        : member_{ std::move(member) }
    { }

    DEFINE_NODE_TYPE(NewExpression);
    std::unique_ptr<Expression> &member() { return member_; }
    bool ProduceRValue() override { return false; }
private:
    std::unique_ptr<Expression> member_; 
};

class PrefixExpression : public Expression {
public:
    PrefixExpression(TokenType tok, std::unique_ptr<Expression> expr)
        : tok_{ tok }, expr_{ std::move(expr) }
    { }
    
    DEFINE_NODE_TYPE(PrefixExpression);

    TokenType op() { return tok_; }
    std::unique_ptr<Expression> &expr() { return expr_; }
private:
    TokenType tok_;
    std::unique_ptr<Expression> expr_;
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(TokenType tok, std::unique_ptr<Expression> expr)
        : tok_{ tok }, expr_{ std::move(expr) }
    { }
    
    DEFINE_NODE_TYPE(PostfixExpression);

    TokenType op() { return tok_; }
    std::unique_ptr<Expression> &expr() { return expr_; }
private:
    TokenType tok_;
    std::unique_ptr<Expression> expr_;
};

class BinaryExpression : public Expression {
public:
    using Operator = TokenType;
    DEFINE_NODE_TYPE(BinaryExpression);

    TokenType op() { return op_; }
    std::unique_ptr<Expression> &lhs() { return lhs_; }
    std::unique_ptr<Expression> &rhs() { return rhs_; }
private:
    Operator op_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
public:
    BinaryExpression(Operator op, std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }
};

class AssignExpression : public Expression {
public:
    AssignExpression(std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs)
        : lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }
    DEFINE_NODE_TYPE(AssignExpression);

    TokenType op() { return op_; }
    std::unique_ptr<Expression> &lhs() { return lhs_; }
    std::unique_ptr<Expression> &rhs() { return rhs_; }
private:
    TokenType op_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class TernaryExpression : public Expression {
public:
    TernaryExpression(std::unique_ptr<Expression> first,
                      std::unique_ptr<Expression> second,
                      std::unique_ptr<Expression> third)
    : first_(std::move(first)), second_(std::move(second)),
        third_(std::move(third))
    { }
    DEFINE_NODE_TYPE(TernaryExpression);

    std::unique_ptr<Expression> &first() { return first_; }
    std::unique_ptr<Expression> &second() { return second_; }
    std::unique_ptr<Expression> &third() { return third_; }
private:
    std::unique_ptr<Expression> first_;
    std::unique_ptr<Expression> second_;
    std::unique_ptr<Expression> third_;
};

class CommaExpression : public Expression {
public:
    CommaExpression(ExpressionList exprs)
    : exprs_{ std::move(exprs) }
    { }
    DEFINE_NODE_TYPE(CommaExpression);

    ExpressionList &exprs() { return exprs_; }
private:
    ExpressionList exprs_;
};

class Declaration : public Expression {
public:
    Declaration(std::string name, std::unique_ptr<Expression> init)
        : name_{ name }, init_{ std::move(init) }
    { }

    Declaration(std::string name)
        : name_{ name }, init_{ nullptr }
    { }
    DEFINE_NODE_TYPE(Declaration);

    std::string &name() { return name_; }

    std::unique_ptr<Expression> &expr() { return init_; }
private:
    std::string name_;
    std::unique_ptr<Expression> init_;
};

class DeclarationList : public Expression {
public:
    DeclarationList(std::vector<std::unique_ptr<Declaration>> exprs)
    : exprs_{ std::move(exprs) }
    { }
    DEFINE_NODE_TYPE(DeclarationList);

    std::vector<std::unique_ptr<Declaration>> &exprs() { return exprs_; }
private:
    std::vector<std::unique_ptr<Declaration>> exprs_;
};

} // namespace parser
} // namespace grok

#endif  /* Expression.h */

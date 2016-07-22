#ifndef AST_VISITOR_H_
#define AST_VISITOR_H_

#include "parser/expression.h"
#include "parser/ifstatement.h"
#include "parser/blockstatement.h"
#include "parser/forstatement.h"
#include "parser/functionstatement.h"
#include "parser/returnstatement.h"

namespace grok {
namespace parser {

class ASTVisitor {
public:
    ASTVisitor() = default;
    virtual ~ASTVisitor() = default;

#define DECLARE_VISITOR_METHOD(type) virtual void Visit(type *) = 0;
AST_NODE_LIST(DECLARE_VISITOR_METHOD)
#undef DECLARE_VISITOR_METHOD
};

class DummyVisitor : public ASTVisitor {
public:
    DummyVisitor() : tab_{ 0 } { }
    std::ostream &stream()
    {
        return std::cout << std::string(tab_, ' ');
    }

    void Visit(NullLiteral *n) override
    {
        stream() << "NullLiteral\n";
    }

    void Visit(ThisHolder *t) override
    {
        stream() << "ThisHolder\n";
    }

    void Visit(IntegralLiteral *i) override
    {
        stream() << "IntegralLiteral\n";
    }

    void Visit(StringLiteral *s) override
    {
        stream() << "StringLiteral\n";
    }

    void Visit(ArrayLiteral *a) override
    {
        ProxyArray &p = a->exprs();
        stream() << "ArrayLiteral\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }
        tab_--;
    }

    void Visit(ObjectLiteral *o) override
    {
        ProxyObject &p = o->proxy();
        tab_++;
        for (auto &prop : p) {
            prop.second->Accept(this);
        }
        tab_--;
    }

    void Visit(Identifier *id) override
    {
        stream() << "Identifier [" << id->GetName() << "]\n";
    }

    void Visit(BooleanLiteral *b) override
    {
        stream() << "BooleanLiteral\n";
    }

    void Visit(ArgumentList *a) override
    {
        ProxyArray &p = a->args();
        stream() << "ArgumentList\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }
        tab_--;
    }

    void Visit(FunctionCallExpression *f) override
    {
        ProxyArray &p = f->args();
        stream() << "FunctionCallExpression\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }

        f->func()->Accept(this);
        tab_--;
    }

    void Visit(CallExpression *c) override
    {
        ProxyArray &p = c->members();
        stream() << "CallExpression\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }
        c->func()->Accept(this);
        tab_--;
    }

    void Visit(DotMemberExpression *d) override
    {
        stream() << "DotMemberExpression\n";
        tab_++;
        d->member()->Accept(this);
        tab_--;
    }

    void Visit(IndexMemberExpression *i) override
    {
        stream() << "IndexMemberExpression\n";
        tab_++;
        i->expr()->Accept(this);
        tab_--;
    }

    void Visit(MemberExpression *m) override
    {
        stream() << "MemberExpression\n";
        tab_++;
        ExpressionList &list = m->members();
        for (auto &expr : list)
            expr->Accept(this);
        tab_--;
    }

    void Visit(NewExpression *n) override
    {
        stream() << "NewExpression\n";
        tab_++;
        n->member()->Accept(this);
        tab_--;
    }

    void Visit(PrefixExpression *p) override
    {
        stream() << "PrefixExpression [" << token_type[p->op()] << "]\n";
        tab_++;
        p->expr()->Accept(this);
        tab_--;
    }

    void Visit(PostfixExpression *p) override
    {
        stream() << "PostfixExpression [" << token_type[p->op()] << "]\n";
        tab_++;
        p->expr()->Accept(this);
        tab_--;
    }

    void Visit(BinaryExpression *b) override
    {
        stream() << "BinaryExpression [" << token_type[b->op()] << "]\n";
        tab_++;
        b->lhs()->Accept(this);
        b->rhs()->Accept(this);
        tab_--;
    }

    void Visit(AssignExpression *a) override
    {
        stream() << "AssignExpression [=]\n";
        tab_++;
        a->lhs()->Accept(this);
        a->rhs()->Accept(this);
        tab_--;
    }

    void Visit(TernaryExpression *t) override
    {
        stream() << "TernaryExpression\n";
        tab_++;
        t->first()->Accept(this);
        t->second()->Accept(this);
        t->third()->Accept(this);
        tab_--;
    }

    void Visit(CommaExpression *) override { }
    void Visit(Declaration *) override { }
    void Visit(DeclarationList *) override { }
    void Visit(IfStatement *) override { }
    void Visit(IfElseStatement *) override { }
    void Visit(ForStatement *) override { }
    void Visit(WhileStatement *) override { }
    void Visit(DoWhileStatement *) override { }
    void Visit(BlockStatement *b) override {
        stream() << "BlockStatement\n";
        tab_++;
        for (auto &stmt : b->statements())
        {
            stmt->Accept(this);
        }
    }
    void Visit(FunctionStatement *) override { }
    void Visit(FunctionPrototype *) override { }
    void Visit(ReturnStatement *) override { }
private:
    int tab_;
};

}
}


#endif

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

    void Visit(NullLiteral *n)
    {
        stream() << "NullLiteral\n";
    }

    void Visit(ThisHolder *t)
    {
        stream() << "ThisHolder\n";
    }

    void Visit(IntegralLiteral *i)
    {
        stream() << "IntegralLiteral\n";
    }

    void Visit(StringLiteral *s)
    {
        stream() << "StringLiteral\n";
    }

    void Visit(ArrayLiteral *a)
    {
        ProxyArray &p = a->exprs();
        stream() << "ArrayLiteral\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }
        tab_--;
    }

    void Visit(ObjectLiteral *o)
    {
        ProxyObject &p = o->proxy();
        tab_++;
        for (auto &prop : p) {
            prop.second->Accept(this);
        }
        tab_--;
    }

    void Visit(Identifier *id)
    {
        stream() << "Identifier [" << id->GetName() << "]\n";
    }

    void Visit(BooleanLiteral *b)
    {
        stream() << "BooleanLiteral\n";
    }

    void Visit(ArgumentList *a)
    {
        ProxyArray &p = a->args();
        stream() << "ArgumentList\n";
        tab_++;
        for (auto &expr : p) {
            expr->Accept(this);
        }
        tab_--;
    }

    void Visit(FunctionCallExpression *f)
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

    void Visit(CallExpression *c)
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

    void Visit(DotMemberExpression *d)
    {
        stream() << "DotMemberExpression\n";
        tab_++;
        d->member()->Accept(this);
        tab_--;
    }

    void Visit(IndexMemberExpression *i)
    {
        stream() << "IndexMemberExpression\n";
        tab_++;
        i->expr()->Accept(this);
        tab_--;
    }

    void Visit(MemberExpression *m)
    {
        stream() << "MemberExpression\n";
        tab_++;
        ExpressionList &list = m->members();
        for (auto &expr : list)
            expr->Accept(this);
        tab_--;
    }

    void Visit(NewExpression *n)
    {
        stream() << "NewExpression\n";
        tab_++;
        n->member()->Accept(this);
        tab_--;
    }

    void Visit(PrefixExpression *p)
    {
        stream() << "PrefixExpression [" << token_type[p->op()] << "]\n";
        tab_++;
        p->expr()->Accept(this);
        tab_--;
    }

    void Visit(PostfixExpression *p)
    {
        stream() << "PostfixExpression [" << token_type[p->op()] << "]\n";
        tab_++;
        p->expr()->Accept(this);
        tab_--;
    }

    void Visit(BinaryExpression *b)
    {
        stream() << "BinaryExpression [" << token_type[b->op()] << "]\n";
        tab_++;
        b->lhs()->Accept(this);
        b->rhs()->Accept(this);
        tab_--;
    }

    void Visit(AssignExpression *a)
    {
        stream() << "AssignExpression [=]\n";
        tab_++;
        a->lhs()->Accept(this);
        a->rhs()->Accept(this);
        tab_--;
    }

    void Visit(TernaryExpression *t)
    {
        stream() << "TernaryExpression\n";
        tab_++;
        t->first()->Accept(this);
        t->second()->Accept(this);
        t->third()->Accept(this);
        tab_--;
    }

    void Visit(CommaExpression *) { }
    void Visit(Declaration *) { }
    void Visit(DeclarationList *) { }
    void Visit(IfStatement *) { }
    void Visit(IfElseStatement *) { }
    void Visit(ForStatement *) { }
    void Visit(WhileStatement *) { }
    void Visit(DoWhileStatement *) { }
    void Visit(BlockStatement *b) {
        stream() << "BlockStatement\n";
        tab_++;
        for (auto &stmt : b->statements())
        {
            stmt->Accept(this);
        }
    }
    void Visit(FunctionStatement *) { }
    void Visit(FunctionPrototype *) { }
    void Visit(ReturnStatement *) { }
private:
    int tab_;
};

}
}


#endif

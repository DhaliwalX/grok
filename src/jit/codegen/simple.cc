#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>

#include "jit/codegen/simple.h"
#include "parser/astvisitor.h"
#include "parser/parser.h"

namespace rl {
#include <readline/readline.h> // conflicts with llvm::Function
#include <readline/history.h>
}

using namespace grok::parser;
using namespace llvm;
using namespace llvm::orc;

static std::unique_ptr<Module> module_;
static std::map<std::string, Value*> named_values_;
static std::unique_ptr<legacy::FunctionPassManager> fpm_;
static std::unique_ptr<JIT> jit_;

class CodeGenerator : public DummyVisitor {
public:
    CodeGenerator(IRBuilder<> &builder) : builder_{ builder } { }
    void Visit(IntegralLiteral *i) override
    {
        stack_.push_back(ConstantFP::get(getGlobalContext(), APFloat(i->value())));
    }

    void Visit(Identifier *i) override
    {
        Value *v = named_values_[i->GetName()];

        if (!v)
            throw grok::ReferenceError("Unknown variable name '" + i->GetName() + "'");
        stack_.push_back(v);
    }

    void CreateBinaryExpression(Value *lhs, Value *rhs, TokenType op)
    {
        Value *v;
        switch (op) {
        case PLUS:
            v = builder_.CreateFAdd(lhs, rhs, "addtmp");
            break;
        case MINUS:
            v = builder_.CreateFSub(lhs, rhs, "subtmp");
            break;
        case MUL:
            v = builder_.CreateFMul(lhs, rhs, "multmp");
            break;
        case DIV:
            v = builder_.CreateFDiv(lhs, rhs, "divtmp");
            break;
        default:
            throw grok::SyntaxError("Unknown operator");
        }
        stack_.push_back(v);
    }

    void Visit(BinaryExpression *b) override
    {
        b->rhs()->Accept(this);
        b->lhs()->Accept(this);
        Value *lhs = stack_.back();
        stack_.pop_back();
        Value *rhs = stack_.back();
        stack_.pop_back();
        CreateBinaryExpression(lhs, rhs, b->op());
    }

    void Visit(BlockStatement *b) override
    {
        std::vector<Type*> doubles(0, Type::getDoubleTy(getGlobalContext()));

        FunctionType *ft =
            FunctionType::get(Type::getDoubleTy(getGlobalContext()), doubles, false);
        Function *f = Function::Create(ft, Function::ExternalLinkage,
                "__anon_expr", module_.get());

        BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "entry", f);
        builder_.SetInsertPoint(bb);

        auto &exprs = b->statements();
        for (auto &expr : exprs) {
            expr->Accept(this);
        }
        Value *ret_val = stack_.back();
        stack_.pop_back();
        builder_.CreateRet(ret_val);

        verifyFunction(*f);

        fpm_->run(*f);
        stack_.push_back(f);
    }

    static void InitializeModuleAndPassManager()
    {
        module_ = llvm::make_unique<Module>("my jit", getGlobalContext());
        module_->setDataLayout(jit_->getTargetMachine().createDataLayout());

        // create a pass manager attached to it.
        fpm_ = llvm::make_unique<legacy::FunctionPassManager>(module_.get());

        // do simple peephole optimizations
        fpm_->doInitialization();
    }

    static void HandleExpression(Expression *expr, CodeGenerator *c)
    {
        // generate code
        expr->Accept(c);
        Value *f = c->value();
        (void)f;
        c->DumpIR();
        auto h = jit_->addModule(std::move(module_));
        InitializeModuleAndPassManager();
        auto symbol = jit_->findSymbol("__anon_expr");

        double (*func)() = (double(*)())(intptr_t)symbol.getAddress();
        std::cout << func() << std::endl;
        jit_->removeModule(h);
    }

    Value *value() { return stack_.back(); }

    void DumpIR() { module_->dump(); }
private:
    std::vector<Value *> stack_;
    IRBuilder<> &builder_;
};

std::string ReadLine(std::string prompt)
{
    using namespace rl;
    char *result = readline(prompt.c_str());
    if (result == 0) {
        return std::string();
    }
    auto ret = std::string(result);

    if (*result) {
        add_history(result);
    }
    free(result);
    return ret;
}

int main()
{
    using namespace llvm;
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    IRBuilder<> builder(getGlobalContext());
    jit_ = llvm::make_unique<JIT>();
    CodeGenerator::InitializeModuleAndPassManager();

    while (true) {
        try {
            std::string str = ReadLine(">> ");
            auto lex = std::make_unique<Lexer>(str);
            grok::parser::GrokParser parser(std::move(lex));
            if (!parser.ParseExpression())
                continue;
            auto ast = parser.ParsedAST();
            CodeGenerator gen(builder);
            CodeGenerator::HandleExpression(ast.get(), &gen);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0; 
}


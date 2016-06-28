#include "grok/runner.h"

#include "grok/context.h"
#include "input/input-stream.h"
#include "input/readline.h"
#include "lexer/lexer.h"
#include "object/jsbasicobject.h"
#include "parser/parser.h"
#include "vm/codegen.h"
#include "vm/context.h"
#include "vm/instruction-list.h"
#include "vm/printer.h"
#include "vm/vm.h"

#include <iostream>
#include <cerrno>

using namespace grok;
using namespace grok::vm;
using namespace grok::parser;
using namespace grok::input;

namespace grok {

int tab_completer(int count, int key)
{
    ReadLineBuffer buffer;
    auto temp = buffer.GetBuffer();
    temp += ' ';
    buffer.SetText(temp);
    return 0;
}

int ExecuteAST(Context *ctx, std::ostream &os, std::shared_ptr<Expression> AST)
{
    try {
        CodeGenerator CG;
        CG.Generate(AST.get());

        auto IR = CG.GetIR();
        if (!IR || IR->size() == 0) 
            return 0;
        if (ctx->DebugInstruction()) {
            Printer P{ os };
            P.Print(IR);
        }
        if (ctx->DryRun())
            return 0;
        auto TheVM = grok::vm::CreateVM(grok::vm::GetGlobalVMContext());
        TheVM->SetCounters(IR->begin(), IR->end());
        
        TheVM->Run();

        Value Result = TheVM->GetResult();
        auto O = GetObjectPointer<grok::obj::JSObject>(Result);

        if (ctx->IsInteractive() || ctx->ShouldPrintLastInStack()) {
            os << O->ToString() << std::endl;
        } else {
            return O->IsTrue();
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

int ExecuteFile(Context *ctx, const std::string &filename, std::ostream &os)
{
    grok::vm::InitializeVMContext();
    std::string v;
    if (FILE *fp = fopen(filename.c_str(), "r"))
    {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), fp))
            v.insert(v.end(), buf, buf + len);
        fclose(fp);
    } else {
        std::cout << "fatal: " << filename << ": "
            << strerror(errno) << std::endl;
        return -1;
    }
    auto lex = std::make_unique<Lexer>(v);
    
    // create parser
    grok::parser::GrokParser parser{ std::move(lex) };
    auto result = parser.ParseExpression();
    if (!result)
        return -1;
    if (ctx->PrintAST()) {
        os << parser << std::endl;
    }

    auto AST = parser.ParsedAST();

    return ExecuteAST(ctx, os, AST);
}

void ExecuteFiles(Context *ctx, std::ostream &os)
{
    auto files = ctx->GetFiles();

    for (auto file : files) {
        ExecuteFile(ctx, file, os);
    }
}

void InteractiveRun(Context *ctx)
{
    grok::vm::InitializeVMContext();
    ReadLine RL{"> "};
    RL.BindKey('\t', tab_completer);
    auto &os = ctx->GetOutputStream();

    // main interpreter loop
    while (true) {
        auto str = RL.Read();
        if (RL.Eof())
            return;
        auto lex = std::make_unique<Lexer>(str);
        GrokParser parser{ std::move(lex) };

        auto result = parser.ParseExpression();
        if (!result)
            continue;

        if (ctx->PrintAST()) {
            os << parser << std::endl;
        }
        auto AST = parser.ParsedAST();
        ExecuteAST(ctx, os, AST);
    }
}

int Start()
{
    auto ctx = GetContext();
    if (ctx->InputViaFile())
        ExecuteFiles(ctx, ctx->GetOutputStream());
    else 
        InteractiveRun(ctx);
    return 0;
}

}


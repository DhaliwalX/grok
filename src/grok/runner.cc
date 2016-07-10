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
#include "common/colors.h"

#include <iostream>
#include <chrono>
#include <cerrno>

using namespace grok;
using namespace grok::vm;
using namespace grok::parser;
using namespace grok::input;

namespace grok {

template<typename Diff>
void log_progress(Diff d)
{
    std::cout << 
    (double)std::chrono::duration_cast
        <std::chrono::microseconds>(d).count() / (double)1000
              << "ms" << std::flush;
}

int tab_completer(int count, int key)
{
    ReadLineBuffer buffer;
    auto temp = buffer.GetBuffer();
    temp += '\t';
    buffer.SetText(temp);
    return 0;
}

int ExecuteAST(Context *ctx, std::ostream &os, std::shared_ptr<Expression> AST)
{
    grok::vm::VM *TheVM = nullptr;
    try {
        CodeGenerator CG;

        auto cgs = std::chrono::high_resolution_clock::now();
        CG.Generate(AST.get());
        auto cge = std::chrono::high_resolution_clock::now();
        
        auto IR = CG.GetIR();
        if (!IR || IR->size() == 0) 
            return 0;
        if (ctx->DebugInstruction()) {
            Printer P{ os };
            P.Print(IR);
        }
        if (ctx->DryRun())
            return 0;
        TheVM = grok::vm::GetGlobalVMContext()->GetVM();
        TheVM->SetCounters(IR->begin(), IR->end());

        auto vms = std::chrono::high_resolution_clock::now();
        TheVM->Run();
        auto vme = std::chrono::high_resolution_clock::now();

        Value Result = TheVM->GetResult();
        auto O = GetObjectPointer<grok::obj::JSObject>(Result);

        if (ctx->IsInteractive() || ctx->ShouldPrintLastInStack()) {
            os << Color::Attr(Color::dim)
                    << O->AsString() << " [ Code generating done in "; 
            log_progress(cge - cgs);
            os << " ] [ Execution took around ";
            log_progress(vme - vms);
            os << " ]" << Color::Reset() << std::endl;
        } else {
            return O->IsTrue();
        }
        TheVM->Reset();
    } catch (std::exception &e) {
        if (TheVM)
            TheVM->Reset();
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

    auto pgs = std::chrono::high_resolution_clock::now();
    auto result = parser.ParseExpression();
    auto pge = std::chrono::high_resolution_clock::now();

    if (!result)
        return -1;
    if (ctx->PrintAST()) {
        os << parser << std::endl;
    }

    auto AST = parser.ParsedAST();
    if (ctx->IsInteractive()) {
        os << Color::Attr(Color::dim) << " [ Parsing done in "; 
        log_progress(pge - pgs);
        os << " ]" << Color::Reset() << std::endl;
    }
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
        if (str == ".debug") {
            ctx->SetDebugInstruction();
            continue;
        } else if (str == ".play") {
            ctx->SetDebugExecution();
            continue;
        }
        if (RL.Eof())
            return;
        auto lex = std::make_unique<Lexer>(str);
        GrokParser parser{ std::move(lex) };


        auto pgs = std::chrono::high_resolution_clock::now();
        auto result = parser.ParseExpression();
        auto pge = std::chrono::high_resolution_clock::now();

        if (!result)
            continue;

        if (ctx->PrintAST()) {
            os << parser << std::endl;
        }
        auto AST = parser.ParsedAST();
        if (ctx->IsInteractive()) {
            os << Color::Attr(Color::dim) << " [ Parsing done in "; 
            log_progress(pge - pgs);
            os << " ]" << Color::Reset() << std::endl;
        }
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


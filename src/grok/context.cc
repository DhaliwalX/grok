#include "grok/context.h"
#include <memory>

namespace grok {

/// ctx ::= main global context
std::unique_ptr<Context> ctx;

void InitializeContext()
{
    ctx = std::make_unique<Context>(std::cout);
    auto O = ctx->GetOptions();
    O->AddOption("help,h", "view help message");
    O->AddOption("interactive,i", "run interactive mode (default if no "
            "other option specified)");
    O->AddOption("instructions", "print the instructions generated");
    O->AddOption("debug-execution", "print the instruction being executed");
    O->AddOption("debug-ast,a", "print AST");
    O->AddOption("dry-run", "don't run the VM");
    O->AddOption("top,t", "print the value present in stack after execution");
    O->AddOption("file,f", "interprete files",
        BPO::value<std::vector<std::string>>()->composing());
    O->AddPositionalOption("file", -1);
}

Context* GetContext()
{
    return ctx.get();
}

void Context::ParseCommandLineOptions(int argc, char **argv)
{
    options.ParseOptions(argc, argv);

    if (options.HasOption("help")) {
        options.PrintHelpAndExit();
    }
    interactive_ = true;

    debug_instruction_ = options.HasOption("instruction");
    debug_execution_ = options.HasOption("debug-execution");

    ast_ = options.HasOption("debug-ast");
    file_ = options.HasOption("file");

    if (file_) {
        files_ = options.GetOptionAs<std::vector<std::string>>("file");
        interactive_ = options.HasOption("interactive");
    }
    dry_run_ = options.HasOption("dry-run");
    last_in_stack_ = options.HasOption("top");
}

void ParseCommandLineOptions(int argc, char **argv)
{
    auto ctx = GetContext();
    ctx->ParseCommandLineOptions(argc, argv);
}

}

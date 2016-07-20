#include "grok/context.h"

#include "object/jsstring.h"
#include "object/function.h"
#include "object/array.h"
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace grok {

/// ctx ::= main global context
std::unique_ptr<Context> ContextStatic::ctx;

Context *ContextStatic::GetContext()
{
    return ctx.get();
}

void ContextStatic::Init()
{
    ctx = std::make_unique<Context>(std::cout);
    ctx->SetVMContext(new grok::vm::VMContext());
    grok::obj::JSObject::Init();
    grok::obj::JSString::Init();
    grok::obj::Function::Init();
    grok::obj::JSArray::Init();
}

void ContextStatic::Teardown()
{
    ctx->RunIO();
}

void InitializeContext()
{
    ContextStatic::Init();
    auto O = ContextStatic::GetContext()->GetOptions();
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
    O->AddOption("profile", "show profiling information while executing");
    O->AddPositionalOption("file", -1);
    GetContext()->SetIOServiceObject();
}

Context* GetContext()
{
    return ContextStatic::GetContext();
}

void Context::ParseCommandLineOptions(int argc, char **argv)
{
    options.ParseOptions(argc, argv);

    if (options.HasOption("help")) {
        options.PrintHelpAndExit();
    }
    interactive_ = true;

    debug_instruction_ = options.HasOption("instructions");
    debug_execution_ = options.HasOption("debug-execution");

    ast_ = options.HasOption("debug-ast");
    file_ = options.HasOption("file");
    profile_ = options.HasOption("profile");

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

void Context::RunIO()
{
    io_.run();
}

void Context::RunPoller()
{
    // while (true) {
    //     boost::asio::deadline_timer timer(io_,
    //             boost::posix_time::microseconds(100));
    //     timer.wait();
    //     io_.poll();
    // }
}

void Context::SetIOServiceObject()
{
}

}

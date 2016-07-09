#include "grok/context.h"
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
    O->AddPositionalOption("file", -1);
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
    std::cout << "Stopped all IO services" << std::endl;
}

void Context::RunPoller()
{
    while (true) {
        boost::asio::deadline_timer timer(io_,
                boost::posix_time::microseconds(1));
        timer.wait();
        std::cout << "Pollling " << std::endl;
        io_.poll();
    }
}

void Context::SetIOServiceObject()
{
    io_thread_ = std::make_unique<std::thread>(std::bind(&Context::RunIO, this));
    poller_thread_ = std::make_unique<std::thread>(
        std::bind(&Context::RunPoller, this));
}

}

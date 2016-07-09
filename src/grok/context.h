#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "vm/context.h"
#include "grok/options.h"

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>

namespace grok {

/// Context ::= stores the global context i.e. various options
class Context {
public:
    Context(std::ostream &os) :
        interactive_{ true }, debug_instruction_{ true },
        debug_execution_{ true }, file_{ false }, ast_{true}, os{ os },
        io_ { },
        work_ { std::make_unique<boost::asio::io_service::work>(io_) }
    { }

    ~Context() = default;

    bool IsInteractive() const { return interactive_; }
    void SetInteractive() { interactive_ = !interactive_; }

    bool DebugInstruction() const { return debug_instruction_; }
    void SetDebugInstruction() { debug_instruction_ = true; }

    bool DebugExecution() const { return debug_execution_; }
    void SetDebugExecution() { debug_execution_ = !debug_execution_; }

    bool ExecuteLineWise() const { return linewise_execute_; }
    void SetExecuteLineWise() { linewise_execute_ = true; }

    bool InputViaFile() const { return file_; }
    void SetInputViaFile() { file_ = true; }

    bool ShouldPrintLastInStack() const { return last_in_stack_; }
    
    decltype(auto) GetFiles() { return files_; }
    void SetInputFiles(std::vector<std::string> files)
    {
        files_ = std::move(files);
    }

    std::ostream &GetOutputStream() { return os; }

    bool PrintAST() const { return ast_; }
    bool DryRun() const { return dry_run_; }

    void ParseCommandLineOptions(int argc, char **argv);

    auto GetOptions() { return &options; }

    void SetIOServiceObject();
    boost::asio::io_service *GetIOService() { return &io_; }
    void RunIO();

    void RunPoller();

    void SetVMContext(grok::vm::VMContext* ctx)
    {
        vmctx_.reset(ctx);
    }

    grok::vm::VMContext *GetVMContext()
    {
        return vmctx_.get();
    }

private:
    bool interactive_; // set the interactive mode
    bool debug_instruction_; // output the instructions generated
    bool debug_execution_; // output the instructions being executed
    bool linewise_execute_; // execute the program linewise
    bool file_; // are we executing file
    std::vector<std::string> files_;
    bool ast_; // print AST
    bool dry_run_;
    bool last_in_stack_;
    std::ostream &os; // output stream used for printing and debugging
    Opts options;

    std::unique_ptr<std::thread> io_thread_;
    std::unique_ptr<std::thread> poller_thread_;
    // boost's asio io_service object for asynchronous events
    boost::asio::io_service io_;
    // to prevent io_.run() from exiting immediately
    std::unique_ptr<boost::asio::io_service::work> work_;

    std::unique_ptr<grok::vm::VMContext> vmctx_;
};

class ContextStatic {
public:
    static Context *GetContext();

    static void Init();
private:
    static std::unique_ptr<Context> ctx;
};

extern void InitializeContext();

extern void InitializeContext(std::ostream &os);

extern void ParseCommandLineOptions(int argc, char **argv);

extern Context* GetContext();

}

#endif

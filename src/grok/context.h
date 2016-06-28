#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "vm/context.h"
#include "grok/options.h"

#include <iostream>
#include <vector>
#include <string>

namespace grok {

/// Context ::= stores the global context i.e. various options
class Context {
public:
    Context(std::ostream &os) :
        interactive_{ true }, debug_instruction_{ true },
        debug_execution_{ true }, file_{ false }, ast_{true}, os{ os }
    { }

    bool IsInteractive() const { return interactive_; }
    void SetInteractive() { interactive_ = true; }

    bool DebugInstruction() const { return debug_instruction_; }
    void SetDebugInstruction() { debug_instruction_ = true; }

    bool DebugExecution() const { return debug_execution_; }
    void SetDebugExecution() { debug_execution_ = true; }

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
};

extern void InitializeContext();

extern void ParseCommandLineOptions(int argc, char **argv);

extern Context* GetContext();

}

#endif

#include "test.h"
#include "test-stats.h"

#include "grok/context.h"
#include "input/input-stream.h"
#include "input/readline.h"
#include "lexer/lexer.h"
#include "object/jsbasicobject.h"
#include "object/argument.h"
#include "object/function.h"
#include "parser/parser.h"
#include "vm/codegen.h"
#include "vm/context.h"
#include "vm/instruction-list.h"
#include "vm/printer.h"
#include "vm/vm.h"
#include "common/colors.h"

#include <iostream>
#include <cerrno>
#include <chrono>
#include <algorithm>
#include <boost/filesystem.hpp>

using namespace grok;
using namespace grok::vm;
using namespace grok::parser;
using namespace grok::input;

namespace grok {
namespace test {

template<typename Diff>
void log_progress(Diff d)
{
    std::cout << 
    (double)std::chrono::duration_cast
        <std::chrono::microseconds>(d).count() / (double)1000
              << "ms" << std::flush;
}

std::shared_ptr<grok::obj::Handle>
    AssertEqual(std::shared_ptr<grok::obj::Argument> args)
{
    auto rhs = args->GetProperty("rhs")->as<grok::obj::JSObject>();
    auto lhs = args->GetProperty("lhs")->as<grok::obj::JSObject>();

    if ((rhs->GetType() == grok::obj::ObjectType::_double
        || rhs->GetType() == grok::obj::ObjectType::_number)
        && (lhs->GetType() == grok::obj::ObjectType::_double
        || lhs->GetType() == grok::obj::ObjectType::_number)
        && (rhs->ToString() == lhs->ToString())
        && (rhs->AsString() == lhs->AsString()))
        return grok::obj::CreateUndefinedObject();
    
    if (rhs->GetType() != lhs->GetType()) {
        throw std::runtime_error(std::string("AssertEqual failed: ")
            + rhs->ToString() + " is not equal to " + lhs->ToString()
            + " types were not same");
    }
    if (rhs->ToString() != lhs->ToString())
        throw std::runtime_error(std::string("AssertEqual failed: ")
            + rhs->ToString() + " is not equal to " + lhs->ToString()
            + " toString's were not same");

    if (rhs->AsString() != lhs->AsString())
        throw std::runtime_error(std::string("AssertEqual failed: ")
            + rhs->ToString() + " is not equal to " + lhs->ToString()
            + " asString's were not same");

    return grok::obj::CreateUndefinedObject();
}

std::shared_ptr<grok::obj::Handle>
    AssertNotEqual(std::shared_ptr<grok::obj::Argument> args)
{
    auto rhs = args->GetProperty("rhs")->as<grok::obj::JSObject>();
    auto lhs = args->GetProperty("lhs")->as<grok::obj::JSObject>();

    if (rhs->GetType() == lhs->GetType()) {
        throw std::runtime_error(std::string("AssertNotEqual failed: ")
            + rhs->ToString() + " is equal to " + lhs->ToString());
    }
    if (rhs->ToString() == lhs->ToString())
        throw std::runtime_error(std::string("AssertNotEqual failed: ")
            + rhs->ToString() + " is equal to " + lhs->ToString());

    if (rhs->AsString() == lhs->AsString())
        throw std::runtime_error(std::string("AssertNotEqual failed: ")
            + rhs->ToString() + " is equal to " + lhs->ToString());

    return grok::obj::CreateUndefinedObject();
}

Test &Test::Prepare(std::string file)
{
    file_ = file;
    expected_result_file_ = file;
    expected_result_file_.resize(file.size() - 3);
    expected_result_file_ += ".txt";

    grok::vm::InitializeVMContext();
    auto v = grok::vm::GetGlobalVMContext()->GetVStore();
    auto func = grok::obj::CreateFunction(AssertEqual);
    func->as<grok::obj::Function>()->SetParams({ "lhs", "rhs" });
    v->StoreValue("assert_equal", func);

    func = grok::obj::CreateFunction(AssertNotEqual);
    func->as<grok::obj::Function>()->SetParams({ "lhs", "rhs" });
    v->StoreValue("assert_not_equal", func);
    return *this;
}

std::string ReadFile(std::string file_)
{
    std::string v;
    if (FILE *fp = fopen(file_.c_str(), "r"))
    {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), fp))
            v.insert(v.end(), buf, buf + len);
        fclose(fp);
    } else {
        std::cout << "fatal: " << file_ << ": "
            << strerror(errno);
        throw std::runtime_error("");
    }
    return v;
}

bool Test::CheckResult(std::string result)
{
    std::string expected_result = ReadFile(expected_result_file_);

    return expected_result == result;
}

bool Test::StartTest(int32_t flags)
{
    std::string file = ReadFile(file_);

    auto lex = std::make_unique<Lexer>(file);

    GrokParser parser{ std::move(lex) };
    if (!parser.ParseExpression())
        return !(flags & f_syntax_errors); // test failed

    auto ast = parser.ParsedAST();

    std::shared_ptr<InstructionList> IR;
    try {
        CodeGenerator codegen;
        codegen.Generate(ast.get());

        IR = codegen.GetIR();
        if (!IR || IR->size() == 0) 
            return true;

    } catch (std::exception &e) {
        if (f_print_reason & flags)
            std::cout << e.what() << std::endl;
        return !(flags & f_ir_errors);
    }

    try {
        auto vm = grok::vm::CreateVM(grok::vm::GetGlobalVMContext());
        vm->SetCounters(IR->begin(), IR->end());
        
        vm->Run();

        Value result = vm->GetResult();
        auto obj = result.O->as<grok::obj::JSObject>();

        std::string res = obj->AsString();

        return true;
    } catch (std::exception &e) {
        if (flags & f_print_reason)
            std::cout << e.what() << std::endl;
        return !(flags & f_vm_errors);
    }
}

}
}

int main()
{
    grok::InitializeContext();
    grok::GetContext()->SetDebugExecution();
    using namespace boost::filesystem;
    using namespace grok::test;

    path p("../test/testing");

    try {
        if (!exists(p)) {
            std::cerr << "There is no test directory! Did you delete it?"
             << std::endl;
        }

        if (!is_directory(p)) {
            std::cerr << "It is odd. 'test' was always a directory containing"
                " many test files but it turns out that 'test' is not a "
                "directory at all." << std::endl;
        }

        // we are good
        std::vector<std::string> files;
        size_t count = 0;
        for (auto &&x : directory_iterator(p)) {
            std::cout << "\rCounting test files: " << count++;
            std::cout << std::flush;
            files.push_back(x.path().string());
        }
        std::cout << std::endl;

        std::sort(files.begin(), files.end());
        TestStats stats;
        size_t i = 0;

        // run the tests
        for (auto file : files) {
            auto s = std::chrono::high_resolution_clock::now();
        
            std::cout << "Testing '" << file + '\'' << std::endl;
            Test test{ grok::GetContext() };

            if (test.Prepare(file).StartTest(f_syntax_errors | f_ir_errors
                    | f_vm_errors | f_print_reason)) {
                std::cout << "Test[" << ++i << "] <"
                    << file << ">: passed.";
                stats.AddPassed();
            } else {
                std::cout << "Test[" << ++i << "] <"
                    << file << ">: failed.";
                stats.AddFailed();
            }

            auto e = std::chrono::high_resolution_clock::now();
            std::cout << "[ time: ";
            log_progress(e - s);
            std::cout << " ]" << std::endl;
        }

        std::cout << stats << std::endl;
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return -1;
}


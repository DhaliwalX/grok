#ifndef TEST_H_
#define TEST_H_

#include "grok/context.h"
#include "test-stats.h"

#include <fstream>
#include <iostream>

namespace grok {
namespace test {

enum TestFlags : int32_t {
    f_syntax_errors = 1,    // test should fail if there is syntax error
    f_ir_errors = 2,    // fail if there are code generation errors
    f_print_ir = 4,     // print IR
    f_vm_errors = 8,    // test should fail if exception was thrown from vm
    f_print_reason = 16,   // print the reason of exception
    f_dont_execute = 2 << 5, // dont execute the code
};

class Test {
public:
    Test(grok::Context *ctx) :
        file_{ "" }, expected_result_file_ { "" }, ctx_{ ctx }
    { }
    
    Test &Prepare(std::string file);

    bool StartTest(int32_t flags);

    bool CheckResult(std::string res);

private:
    std::string file_;
    std::string expected_result_file_;
    grok::Context *ctx_;
};

}
}

#endif

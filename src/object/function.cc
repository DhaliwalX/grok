#include "object/function.h"
#include "vm/codegen.h"

namespace grok {
namespace obj {
using namespace grok::vm;
using namespace grok::parser;

bool Function::HasCodeGened() const 
{
    return Native || CodeGened;
}

void Function::PrepareFunction()
{
    if (HasCodeGened())
        return;

    if (!AST)
        throw std::runtime_error("there was no function prototype");

    CodeGenerator Generator;
    Generator.SetInsideFunction();
    Generator.Generate(AST.get());
    IR = Generator.GetIR();
    CodeGened = true;
}

bool Function::IsNative() const 
{
    return Native;
}

Counter Function::GetAddress()
{
    // instead of returning only beginning we could have returned end
    // also so to avoid iterator overflow, but during code generation
    // stores a leave instruction at the end. So we don't have to worry
    // about that.
    return IR->begin();
}

const std::vector<std::string> &Function::GetParams() const
{
    return Proto->GetArgs();
}

}
}

#include "object/function.h"
#include "object/argument.h"
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
    // stores a ret instruction at the end. So we don't have to worry
    // about that.
    return IR->begin();
}

const std::vector<std::string> &Function::GetParams() const
{
    return Params;
}

Value Function::CallNative(std::vector<grok::vm::Value> Args,
    std::shared_ptr<JSObject> This)
{
    auto Obj = CreateArgumentObject();
    auto ArgObj = Obj->as<Argument>();
    size_t idx = 0, max = 0;
    auto vec = Params;

    max = vec.size();
    for (auto i : Args) {
        ArgObj->Push(i.O);

        // we don't want to lose extra args
        if (idx < max)
            ArgObj->AddProperty(vec[idx++], i.O);
    }

    while (idx < max) {
        ArgObj->AddProperty(vec[idx++], CreateUndefinedObject());
    }

    auto ThisWrapped = std::make_shared<Object>(This);
    ArgObj->AddProperty(std::string("this"), ThisWrapped);
    // call the native function
    auto ret = NFT(ArgObj);

    auto wrapped = Value(ret);
    return wrapped;
}

std::shared_ptr<Object> CallJSFunction(std::shared_ptr<Function> func,
        std::shared_ptr<Argument> Args, VM* vm)
{
    func->PrepareFunction();

    // save the `this` function to the stack
    vm->PushArg(std::make_shared<Object>(func));

    for (auto Arg : *Args) {
        vm->PushArg(Arg);
    }

    auto instr = std::make_shared<Instruction>();
    instr->kind_ = Instructions::call;
    instr->data_type_ = d_num;
    instr->number_ = Args->Size();

    // transfer the control
    vm->ExecuteInstruction(instr);

    if (!func->IsNative()) {        

        // now we are handling the VM by our own
        auto Beg = func->GetAddress();
        auto End = func->GetEnd();

        while (Beg != End) {
            vm->ExecuteInstruction(*Beg);

            if ((*Beg)->GetKind() == Instructions::ret)
                break;
            Beg++;
        }
    }
    // get the return value
    auto result = vm->GetResult();
    return result.O;
}

}
}

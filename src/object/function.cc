#include "object/function.h"
#include "object/argument.h"
#include "vm/codegen.h"
#include "vm/vm.h"

#include <sstream>

namespace grok {
namespace obj {
using namespace grok::vm;
using namespace grok::parser;

Function::Function()
    : JSObject{}, AST{}, Proto{ nullptr }, NFT{ nullptr },
      Native{ false }, CodeGened{ false }, IR{}, Params{ }
{ }

Function::Function(std::shared_ptr<grok::parser::Expression> AST,
    std::shared_ptr<grok::parser::FunctionPrototype> proto)
    : JSObject(), AST{ AST }, Proto{ proto }, NFT{ nullptr },
      Native{ false }, CodeGened{ false }, IR{}, Params{ Proto->GetArgs() }
{ }

Function::Function(NativeFunctionType function)
    : JSObject{}, AST{}, Proto{ nullptr }, NFT{ function },
      Native{ true }, CodeGened{ true }, IR{}, Params{}
{ }

std::string Function::AsString() const
{
    std::ostringstream os;
    if (Native) {
        os << "function() { [native code] }";
        return os.str();
    }

    os << "function " << Proto->GetName();
    os << "(";
    auto Args = GetParams();
    if (Args.size()) {
        for (auto Arg = Args.cbegin(); Arg != Args.cend() - 1; Arg++)
        {
            os << *Arg << ", ";
        }
    }
    if (Args.size() > 0) {
        os << Args.back();
    }
    os << ") { [Code] }";
    return os.str();
}

std::string Function::ToString() const
{
    return "[ function Function ]";
}

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

void CreatePushInstruction(InstructionBuilder *b,
    std::shared_ptr<Handle> object)
{
    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_obj;
    instr->data_ = object;

    b->AddInstruction(std::move(instr));
}

std::vector<std::shared_ptr<InstructionList>> st;

void CreateInterruptRequest(std::shared_ptr<Function> func,
        std::shared_ptr<Argument> Args, VM* vm)
{
    auto ib = InstructionBuilder::CreateBuilder();
    ib->CreateBlock();

    CreatePushInstruction(ib.get(), std::make_shared<Handle>(func));

    for (auto Arg : *Args) {
        CreatePushInstruction(ib.get(), Arg);
    }

    auto instr = std::make_shared<Instruction>();
    instr->kind_ = Instructions::call;
    instr->data_type_ = d_num;
    instr->number_ = Args->Size();
    ib->AddInstruction(std::move(instr));
    
    ib->Finalize();
    std::shared_ptr<InstructionList> ir = ib->ReleaseInstructionList();
    st.push_back(ir);
    // transfer the control
    // TODO: The result of this interrupt must be stored somewhere
    vm->RequestInterrupt(ir->begin(), ir->end());

    if (!vm->IsRunning()) {
        vm->HandleInterrupt();
    }
}

}
}

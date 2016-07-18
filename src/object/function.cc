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

Value Function::CallNative(std::vector<grok::vm::Value> &Args,
    std::shared_ptr<Handle> This)
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

    ArgObj->AddProperty(std::string("this"), This);
    // call the native function
    auto ret = NFT(ArgObj);

    auto wrapped = Value(ret);
    return wrapped;
}

void CreatePushInstruction(InstructionBuilder *b,
    std::shared_ptr<Handle> object)
{
    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_obj;
    instr->data_ = object;

    b->AddInstruction(std::move(instr));
}

std::shared_ptr<Object> CallJSFunction(std::shared_ptr<Function> func,
        std::shared_ptr<Argument> Args, VM* vm, bool with_this)
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

    // transfer the control
    vm->SaveState();

    // set this to proper value
    if (with_this) {
        auto this_handle = Args->GetProperty("this");
        vm->SetThis(this_handle);
    } else {
        vm->SetThisGlobal();
    }

    vm->SetCounters(ir->begin(), ir->end());
    vm->Run();
    // get the return value
    auto result = vm->GetResult();
    vm->RestoreState();

    return result.O;
}

void CreateInterruptRequest(std::shared_ptr<Function> func,
        std::shared_ptr<Argument> Args, VM* vm)
{
    static std::vector<std::shared_ptr<InstructionList>> st;
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

std::shared_ptr<Handle> JSFunctionApply(std::shared_ptr<Argument> args)
{
    auto this_function_handle = args->GetProperty("this");

    if (!IsFunction(this_function_handle)) {
        return CreateUndefinedObject();
    }
    auto this_function = this_function_handle->as<Function>();
    auto this_object_handle = args->At(0);
    auto arg_handle = args->At(1);

    auto arg_object = arg_handle->as<JSObject>();
    auto arg_length_handle = arg_object->GetProperty("length");

    std::shared_ptr<Handle> args_to_pass_handle = CreateArgumentObject();
    auto args_to_pass = args_to_pass_handle->as<Argument>();
    args_to_pass->AddProperty("this", this_object_handle);
    if (!IsUndefined(arg_length_handle)) {
        size_t args_length = arg_length_handle->as<JSDouble>()->GetNumber();

        for (size_t i = 0; i < args_length; i++) {
            auto prop = arg_object->GetProperty(std::to_string(i));
            args_to_pass->Push(prop);
        }
    }

    return CallJSFunction(this_function, args_to_pass,
                GetGlobalVMContext()->GetVM(), true);
}

std::shared_ptr<Handle> JSFunctionCallMethod(std::shared_ptr<Argument> args)
{
    auto this_function_handle = args->GetProperty("this");

    if (!IsFunction(this_function_handle)) {
        return CreateUndefinedObject();
    }
    auto this_function = this_function_handle->as<Function>();
    auto this_object_handle = args->At(0);

    std::shared_ptr<Handle> args_to_pass_handle = CreateArgumentObject();
    auto args_to_pass = args_to_pass_handle->as<Argument>();
    args_to_pass->AddProperty("this", this_object_handle);

    for (size_t i = 1; i < args->Size(); i++) {
        args_to_pass->Push(args->At(i));
    }

    return CallJSFunction(this_function, args_to_pass,
                GetGlobalVMContext()->GetVM(), true);
}

std::shared_ptr<Handle> Function::st_func_handle    ;

void Function::Init()
{
    st_func_handle = CreateFunction(nullptr);
    auto st_func = st_func_handle->as<Function>();

    auto function_handle = CreateFunction(JSFunctionApply);
    st_func->AddProperty("apply", function_handle);

    function_handle = CreateFunction(JSFunctionCallMethod);
    st_func->AddProperty("call", function_handle);
}

std::pair<std::shared_ptr<Handle>, bool>
 Function::GetStaticProperty(const std::string &str)
{
    if (str == "apply")
        return { CreateFunction(JSFunctionApply), true };

    if (str == "call")
        return { CreateFunction(JSFunctionCallMethod), true };
    auto st_func = st_func_handle->as<Function>();

    if (!st_func->HasProperty(str))
        return { nullptr, false };
    auto prop = st_func->JSObject::GetProperty(str);
    return { prop, true };
}

std::shared_ptr<Handle> Function::GetProperty(const std::string &str)
{
    auto p = GetStaticProperty(str);
    if (p.second) {
        return p.first;
    }

    return JSObject::GetProperty(str);
}

}
}

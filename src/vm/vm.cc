#include "vm/vm.h"
#include "object/jsbasicobject.h"
#include "object/jsobject.h"
#include "object/array.h"
#include "object/object.h"
#include "object/function.h"

#include <algorithm>

namespace grok {
namespace vm {

using namespace grok::obj;

std::unique_ptr<VM> CreateVM(VMContext *context)
{
    auto ret = std::unique_ptr<VM>{ new VM() };
    ret->SetContext(context);
    return std::move(ret);
}

void VM::SetContext(VMContext *context)
{
    Context = context;
}

Value VM::GetResult()
{
    if (Stack.empty()) {
        return Value();
    }
    auto V = Stack.Top();
    return V;
}

void VM::SetCounters(Counter start, Counter end)
{
    Current = start;
    End = end;
}

void VM::SaveState()
{
    CStack.Push(Current);
    FStack.Push(Flags);
    HelperStack.Push(Stack.size());
}

void VM::RestoreState()
{
    Current = CStack.Pop();
    Flags = FStack.Pop();
    Stack.resize(HelperStack.Pop());
}

void VM::SetAC(Value v)
{
    AC = v;
}

void VM::NoOP()
{
    // really its noop
}

void VM::FetchOP()
{
    if (GetCurrent()->GetDataType() != d_name) {
        throw std::runtime_error("BUG: fetch instruction");
    }

    VStore *V = GetVStore(Context);
    auto name = GetCurrent()->GetString();

    if (name.length() == 0)
        throw std::runtime_error("fatal: name of a variable can't be \"\"");
    Value TheValue = V->GetValue(name);
    SetAC(TheValue);
}

void VM::StoreOP()
{
    auto LHS = Stack.Pop();
    auto RHS = Stack.Pop();
    if (LHS.O->as<JSObject>()->IsWritable())
        LHS.O->Reset(*(RHS.O));
    Stack.Push(LHS);
    SetFlags();
}

std::shared_ptr<grok::obj::JSObject> VM::GetThis()
{
    if (TStack.Empty()) {
        auto V = GetVStore(Context);
        return V->This();
    }
    return TStack.Top();
}

void VM::PushNumber(double number)
{
    auto V_N_ = CreateJSNumber(number);
    Stack.Push(V_N_);
    SetFlags();
}

void VM::PushString(const std::string &str)
{
    auto V_S_ = CreateJSString(str);
    Stack.Push(V_S_);
    SetFlags();
}

void VM::PushNull()
{
    auto N = std::make_shared<JSNull>();
    auto V_N_ = std::make_shared<Object>(N);
    Stack.Push(V_N_);
    SetFlags();
}

void VM::PushBool(bool boolean)
{
    auto V_B_ = CreateJSNumber(boolean);
    Stack.Push(V_B_);
    SetFlags();
}

void VM::PushOP()
{
    switch (GetCurrent()->GetDataType()) {
    default:
        throw std::runtime_error("Unknown type was asked to push");
    case d_num:
        PushNumber(GetCurrent()->GetNumber());
        break;
    case d_str:
        PushString(GetCurrent()->GetString());
        break;

    case d_bool:
        PushBool(GetCurrent()->boolean_);
        break;
    case d_null:
        PushNull();
    }
}

void VM::PushimOP()
{
    Stack.Push(AC);
    SetFlags();
}

void VM::PushthisOP()
{
    Stack.Push(std::make_shared<grok::obj::Object>(TStack.Pop()));
}

/// opopopopopop
void VM::PoppropOP()
{
    auto O = CreateJSObject();
    auto Obj = O->as<JSObject>();
    auto Sz = static_cast<std::size_t>(GetCurrent()->GetNumber());
    for (decltype(Sz) i = 0; i < Sz; i++) {
        auto Prop = Stack.Pop();
        if (Prop.S->length() == 0)
            throw std::runtime_error("Property name length was 0");
        Obj->AddProperty(*Prop.S, Prop.O);
    }

    Stack.Push(O);
    SetFlags();
}

void VM::ReplpropOP()
{
    auto MayBeObject = Stack.Pop();
    auto Object = GetObjectPointer<JSObject>(MayBeObject);
    auto Name = GetCurrent()->GetString();

    TStack.Push(Object);
    auto Prop = Object->GetProperty(Name);
    Stack.Push(Prop);
    SetFlags();
}

void VM::IndexArray(std::shared_ptr<JSArray> arr, std::shared_ptr<Object> obj)
{
    auto prop = obj->as<JSObject>()->ToString();
    auto val = arr->At(prop);
    Stack.Push(val);
}

void VM::IndexObject(std::shared_ptr<JSObject> obj, std::shared_ptr<Object> idx)
{
    auto prop = idx->as<JSObject>()->ToString();
    auto val = obj->GetProperty(prop);
    Stack.Push(val);
}

void VM::IndexOP()
{
    auto index = Stack.Pop().O;
    auto Unknown = Stack.Pop().O;

    if (IsJSArray(Unknown)) {
        auto Array = GetObjectPointer<JSArray>(Unknown);
        IndexArray(Array, index);
    } else {
        auto Object = GetObjectPointer<JSObject>(Unknown);
        IndexObject(Object, index);
    }
    SetFlags();
}

void VM::ResOP()
{
    auto Sz = static_cast<size_t>(GetCurrent()->GetNumber());
    auto Array = CreateArray(Sz);
    SetAC(Array);
}

void VM::NewsOP()
{
    auto name = GetCurrent()->GetString();
    auto var = CreateUndefinedObject();
    auto V = GetVStore(Context);
    V->StoreValue(name, var);
}

void VM::CpyaOP()
{
    auto Array = GetObjectPointer<JSArray>(AC);

    auto Sz = static_cast<size_t>(GetCurrent()->GetNumber());

    for (decltype(Sz) i = Sz; i > 0; i--) {
        Array->Assign(i - 1, Stack.Pop().O);
    }
}

void VM::MapsOP()
{
    auto S = std::make_shared<std::string>(GetCurrent()->GetString());
    Stack.Top().S = S;
}

void VM::SetFlags()
{
    auto Obj = GetObjectPointer<JSObject>(Stack.Top());

    if (!Obj->IsTrue()) 
        Flags |= zero_flag;
    else 
        Flags &= ~zero_flag;

}

void VM::AddsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS + RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::SubsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS - RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::MulsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS * RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::DivsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS / RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::RemsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS % RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::GtsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS > RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::LtsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS < RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::EqsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS == RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::NeqsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS != RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::ShlsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS << RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::ShrsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS >> RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::BorsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS | RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::BandsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS & RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::OrsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS || RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::AndsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS && RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::XorsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Object>(LHS ^ RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::IncOP()
{
    auto RHS = Stack.Pop().O;
    auto str = RHS->as<JSObject>()->ToString();
    auto num = CreateJSNumber(str);
    if (!IsUndefined(num)) {
        ++num->as<JSNumber>()->GetNumber();
    }
    RHS->Reset(*num);
    Stack.Push(RHS);
    SetFlags();
}

void VM::DecOP()
{
    auto RHS = Stack.Pop().O;
    auto str = RHS->as<JSObject>()->ToString();
    auto num = CreateJSNumber(str);
    if (!IsUndefined(num)) {
        --num->as<JSNumber>()->GetNumber();
    }
    RHS->Reset(*num);
    Stack.Push(RHS);
    SetFlags();
}

void VM::PincOP()
{
    auto RHS = Stack.Pop().O;
    auto str = RHS->as<JSObject>()->ToString();
    auto num = CreateJSNumber(str);
    auto res = CreateJSNumber(str);
    if (!IsUndefined(num)) {
        ++num->as<JSNumber>()->GetNumber();
    }
    RHS->Reset(*num);
    Stack.Push(res);
    SetFlags();
}

void VM::PdecOP()
{
    auto RHS = Stack.Pop().O;
    auto str = RHS->as<JSObject>()->ToString();
    auto num = CreateJSNumber(str);
    auto res = CreateJSNumber(str);
    if (!IsUndefined(num)) {
        --num->as<JSNumber>()->GetNumber();
    }
    RHS->Reset(*num);
    Stack.Push(res);
    SetFlags();
}

void VM::JmpOP()
{
    Current += GetCurrent()->jmp_addr_; 
}

void VM::JmpzOP()
{
    if (Flags & zero_flag)
        JmpOP();
}

PassedArguments VM::CreateArgumentList(size_t sz)
{
    PassedArguments Args;

    Args.resize(sz);
    while (sz--) {
        Args[sz] = Stack.Pop();
    }

    return std::move(Args);
}

void VM::MarkstOP()
{
    Flags |= constructor_call;
}

void VM::CallOP()
{
    auto Args = CreateArgumentList(GetCurrent()->GetNumber());
    auto F = Stack.Pop();
    
    SaveState();
    if (!IsFunction(F.O))
        throw std::runtime_error("fatal: not a function");

    auto TheFunction = F.O->as<Function>();

    TheFunction->PrepareFunction();

    if (TheFunction->IsNative()) {
        auto This = GetThis();
        auto ret = TheFunction->CallNative(Args, This);
        Stack.Push(ret);
        return;
    }

    // all the variables for this function will lie in a new scope
    auto V = GetVStore(Context);
    V->CreateScope();

    // initialize parameters of the function
    auto Params = TheFunction->GetParams();

    auto PSz = Params.size();
    auto ASz = Args.size();
    auto Sz = std::min(PSz, ASz);

    for (auto i = decltype(Sz)(0); i < Sz; ++i) {
        V->StoreValue(Params[i], Args[i]);
    }

    // now we are in position to transfer the control
    // to the function
    Current = TheFunction->GetAddress();
}

void VM::RetOP()
{
    // save the returned result
    auto ReturnedValue = Stack.Pop();
    RestoreState();
    
    auto V = GetVStore(Context);
    // we check that whether we called a constructor
    // if yes then we will save the object created on to stack
    if (Flags & constructor_call) {
        auto This = V->This();
        TStack.Push(This);
    }

    // Save the return value
    Stack.Push(ReturnedValue);
    V->RemoveScope();
}

void VM::LeaveOP()
{
    Current = End;
}

void VM::PrintCurrentState()
{
    std::cout << InstructionToString(**Current) << " ";
    std::cout << (Flags & zero_flag ? "Z " : " ") << std::endl;
}

void VM::ExecuteInstruction(std::shared_ptr<Instruction> instr)
{
    switch (instr->GetKind()) {
    case Instructions::noop:
        NoOP();
        break;
    case Instructions::fetch:
        FetchOP();
        break;
    case Instructions::store:
        StoreOP();
        break;
    case Instructions::push:
        PushOP();
        break;
    case Instructions::pushim:
        PushimOP();
        break;
    case Instructions::poprop:
        PoppropOP();
        break;
    case Instructions::replprop:
        ReplpropOP();
        break;
    case Instructions::index:
        IndexOP();
        break;
    case Instructions::res:
        ResOP();
        break;
    case Instructions::news:
        NewsOP();
        break;
    case Instructions::cpya:
        CpyaOP();
        break;
    case Instructions::maps:
        MapsOP();
        break;
    case Instructions::lts:
        LtsOP();
        break;
    case Instructions::gts:
        GtsOP();
        break;
    case Instructions::eqs:
        EqsOP();
        break;
    case Instructions::neqs:
        NeqsOP();
        break;
    case Instructions::adds:
        AddsOP();
        break;
    case Instructions::subs:
        SubsOP();
        break;
    case Instructions::muls:
        MulsOP();
        break;
    case Instructions::divs:
        DivsOP();
        break;
    case Instructions::shls:
        ShlsOP();
        break;
    case Instructions::shrs:
        ShrsOP();
        break;
    case Instructions::rems:
        RemsOP();
        break;
    case Instructions::bors:
        BorsOP();
        break;
    case Instructions::bands:
        BandsOP();
        break;
    case Instructions::ors:
        OrsOP();
        break;
    case Instructions::ands:
        AndsOP();
        break;
    case Instructions::xors:
        XorsOP();
        break;
    case Instructions::loopz:
        
        break;
    case Instructions::jmp:
        JmpOP();
        break;
    case Instructions::call:
        CallOP();
        break;
    case Instructions::ret:
        RetOP();
        break;
    case Instructions::jmpz:
        JmpzOP();
        break;
    case Instructions::leave:
        LeaveOP();
        break;
    case Instructions::markst:
        MarkstOP();
        break;
    case Instructions::pushthis:
        PushthisOP();
        break;
    case Instructions::inc:
        IncOP();
        break;
    case Instructions::dec:
        DecOP();
        break;
    case Instructions::pinc:
        PincOP();
        break;
    case Instructions::pdec:
        PdecOP();
        break;
    }
}

void VM::Run()
{
    // main loop
    while (Current != End) {
        if (debug_execution_)
            PrintCurrentState();
        ExecuteInstruction(GetCurrent());
        ++Current;
    }
}

} // vm
} // grok

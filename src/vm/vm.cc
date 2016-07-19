#include "vm/vm.h"
#include "object/jsbasicobject.h"
#include "object/jsobject.h"
#include "object/array.h"
#include "object/object.h"
#include "object/function.h"
#include "object/prototype.h"

#include <algorithm>
#include <bitset>

namespace grok {
namespace vm {

using namespace grok::obj;

std::unique_ptr<VM> CreateVM(VMContext *context)
{
    auto ret = std::unique_ptr<VM>{ new VM() };
    ret->SetContext(context);
    context->SetVM(ret.get());
    return (ret);
}

void VM::ShutDown()
{
    Context = nullptr;
    Current = Start;

    Stack.clear();
    TStack.clear();
    this_helper.clear();
    HelperStack.clear();
    CStack.clear();
    FStack.clear();
}

void VM::Reset()
{
    Current = Start;

    Stack.clear();
    TStack.clear();
    this_helper.clear();
    HelperStack.clear();
    CStack.clear();
    FStack.clear();
}

void VM::SetContext(VMContext *context)
{
    Context = context;
    auto V = GetVStore(Context);
    js_this_ = (V->This());
}

Value VM::GetResult()
{
    if (Stack.empty()) {
        return CreateUndefinedObject();
    }
    auto V = Stack.Pop();
    return V;
}

void VM::SetThis(std::shared_ptr<grok::obj::Handle> obj)
{
    js_this_ = obj;
}

void VM::SetThisGlobal()
{
    auto V = GetVStore(Context);
    js_this_ = (V->This());
}

bool VM::IsConstructorCall()
{
    return Flags & constructor_call;
}

void VM::EndedConstructorCall()
{
    Flags &= ~constructor_call;
}

/// Here we can set the counters i.e. Program Counter (PC)
/// or instruction register to the first instruction which we want to 
/// execute and End points to the last instruction in the list so we know
/// when we want to exit.
void VM::SetCounters(Counter start, Counter end)
{
    Current = start;
    Start = start;
    End = end;
}

void VM::RequestInterrupt(Counter start, Counter end)
{
    RQ.Push({ start, end });
}

/// When a function call takes place we have to save the current position
/// of our instruction register, current instruction we are executing,
/// flags and a size of stack before the function call because stack grows
/// in execution of each expression i.e. result is never popped from the 
/// stack. Consider for example.
///             a + b;
/// as usual instruction generated will be
///             push a
///             push b
///             add
/// Even we don't need to save the result after the statement has been executed
/// But still the result will remain in stack thus eating up RAM. We can save
/// bit of RAM by removing the result when function has returned by resizing
/// the stack to its previous size when it was before function call. That size
/// is saved in HelperStack
void VM::SaveState()
{
    CStack.Push(Current);
    CStack.Push(End);
    FStack.Push(Flags);
    HelperStack.Push(Stack.size());
    this_helper.Push(TStack.size());
}

/// RestoreState restores the state of the stacks & counters after function
/// call
void VM::RestoreState()
{
    End = CStack.Pop();
    Current = CStack.Pop();
    Flags = FStack.Pop();
    Stack.resize(HelperStack.Pop());
    TStack.resize(this_helper.Pop());
}

void VM::SetAC(Value v)
{
    AC = v;
}

void VM::NoOP()
{
    // really its noop
}

/// Though it is a wasteful instruction because fetch and pushim always
/// work together just after fetch has been executed the pushim is guaranteed
/// that it will be executed after. See genexpression.cc
void VM::FetchOP()
{
    VStore *V = GetVStore(Context);
    auto name = GetCurrent()->GetString();

    if (name == "this") {
        SetAC(GetThis());
        return;
    }
    Value TheValue = V->GetValue(name);
    SetAC(TheValue);
}

/// Stores the value at the top - 1 position of the stack into the
/// top position. We also check whether or not the object we are 
/// wrting is writable or not. Thus supporting const like objects
void VM::StoreOP()
{
    auto LHS = Stack.Pop();
    auto RHS = Stack.Pop();
    if (LHS.O->as<JSObject>()->IsWritable())
        LHS.O->Reset(*CreateCopy(RHS.O));
    Stack.Push(LHS);
    SetFlags();
}

/// `this` in javascript is a tricky keyword. When code is executed globally
/// it points to a global object. But inside a function its value may change.
/// See 
// https://developer.mozilla.org/en/docs/Web/JavaScript/Reference/Operators/this
/// Nearly every variable declared outside function is bound to global object
// TODO: Find a better way to do this...
std::shared_ptr<grok::obj::Object> VM::GetThis()
{
    return js_this_;
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
    auto V_N_ = std::make_shared<Handle>(N);
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
        break;
    case d_obj: {
        std::shared_ptr<Handle> obj = GetCurrent()->GetData();
        Stack.Push(Value(obj));
    }
    }
}

void VM::PopOP()
{
    Stack.Pop();
    // pop instruction doesn't affect flags
}

void VM::PushimOP()
{
    Stack.Push(AC);
    SetFlags();
}

/// `this` always lies in TStack 
void VM::PushthisOP()
{
    Stack.Push((TStack.Pop()));
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
    auto Obj = GetObjectPointer<JSObject>(MayBeObject);
    auto Name = GetCurrent()->GetString();

    auto Prop = Obj->GetProperty(Name);
    Stack.Push(Prop);
    member_ = MayBeObject.O;
    SetFlags();
}

void VM::IndexArray(std::shared_ptr<JSArray> arr, std::shared_ptr<Handle> obj)
{
    auto prop = obj->as<JSObject>()->ToString();
    auto val = arr->At(prop);
    Stack.Push(val);
}

void VM::IndexObject(std::shared_ptr<JSObject> obj, std::shared_ptr<Handle> idx)
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
    member_ = Unknown;
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
    auto Obj = Stack.Top().O->as<JSObject>();

    if (!Obj->IsTrue()) 
        Flags |= zero_flag;
    else 
        Flags &= ~zero_flag;

}

void VM::AddsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS + RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::SubsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS - RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::MulsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS * RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::DivsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS / RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::RemsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS % RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::GtsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS > RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::LtsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS < RHS);
    Stack.Push(Result);
    SetFlags();
}
void VM::GtesOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS >= RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::LtesOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS <= RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::EqsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS == RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::NeqsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS != RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::ShlsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS << RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::ShrsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS >> RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::BorsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS | RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::BandsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS & RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::OrsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS || RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::AndsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS && RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::XorsOP()
{
    auto RHS = *(Stack.Pop().O);
    auto LHS = *(Stack.Pop().O);
    auto Result = std::make_shared<Handle>(LHS ^ RHS);
    Stack.Push(Result);
    SetFlags();
}

void VM::IncOP()
{
    auto RHS = Stack.Pop().O;
    if (!IsJSNumber(RHS)) {
        throw ReferenceError("can't apply increment operator");
    }
    auto num = RHS->as<JSDouble>();
    ++num->GetNumber();
    Stack.Push(RHS);
    SetFlags();
}

void VM::DecOP()
{
    auto RHS = Stack.Pop().O;
    if (!IsJSNumber(RHS)) {
        throw ReferenceError("can't apply increment operator");
    }
    auto num = RHS->as<JSDouble>();
    --num->GetNumber();
    Stack.Push(RHS);
    SetFlags();
}

void VM::SnotOP()
{
    auto RHS = Stack.Pop().O;
    auto obj = RHS->as<JSObject>();
    auto num = CreateJSNumber(!obj->IsTrue());
    Stack.Push(num);
    SetFlags();
}

void VM::BnotOP()
{
    auto RHS = Stack.Pop().O;
    if (!IsJSNumber(RHS)) {
        throw ReferenceError("can't apply '~' operator");
    }
    auto num = RHS->as<JSDouble>();
    auto res = CreateJSNumber(~(int32_t)num->GetNumber());
    Stack.Push(res);
    SetFlags();
}

void VM::PincOP()
{
    auto RHS = Stack.Pop().O;
    if (!IsJSNumber(RHS)) {
        throw ReferenceError("can't apply '++' operator");
    }
    auto num = RHS->as<JSDouble>();
    auto res = CreateJSNumber(num->GetNumber()++);
    Stack.Push(res);
    SetFlags();
}

void VM::PdecOP()
{
    auto RHS = Stack.Pop().O;
    if (!IsJSNumber(RHS)) {
        throw ReferenceError("can't apply '--' operator");
    }
    auto num = RHS->as<JSDouble>();
    auto res = CreateJSNumber(num->GetNumber()--);
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

void VM::JmpnzOP()
{
    if (!(Flags & zero_flag))
        JmpOP();
}

PassedArguments VM::CreateArgumentList(size_t sz)
{
    PassedArguments Args;

    Args.resize(sz);
    while (sz--) {
        Args[sz] = Stack.Pop();
    }

    return (Args);
}

void VM::MarkstOP()
{
    Flags |= constructor_call;
}

void VM::CallNative(std::shared_ptr<Handle> function_object,
    PassedArguments &Args)
{
    auto function = function_object->as<Function>();

    auto This = GetThis();
    auto ret = function->CallNative(Args, This);
    Flags = FStack.Pop();
    CStack.Pop();
    CStack.Pop();
    HelperStack.Pop();

    if (IsConstructorCall()) {
        js_this_ = TStack.Pop();
        EndedConstructorCall();
    } else if (IsMemberCall()) {
        js_this_ = TStack.Pop();
        EndMemberCall();
    } else {
        js_this_ = TStack.Pop();
    }
    Stack.Push(ret);
    SetFlags();
}

bool VM::CallPrologue()
{
    auto Args = CreateArgumentList(GetCurrent()->GetNumber());
    auto F = Stack.Pop();

    SaveState();
    if (!IsFunction(F.O))
        throw std::runtime_error("fatal: not a function");

    auto TheFunction = F.O->as<Function>();
    TheFunction->PrepareFunction();

    if (Flags & constructor_call) {
        auto newthis_wrapped = CreateJSObject();

        // we now copy the properties of the constuctor.prototype
        // native functions have to copy the properies by their own
        auto newthis = newthis_wrapped->as<JSObject>();
        AddPropertiesFromPrototype(TheFunction.get(), newthis.get());
        TStack.Push(js_this_);
        js_this_ = newthis_wrapped;
    } else if (IsMemberCall()) {
        TStack.Push(js_this_);
        js_this_ = member_;
        EndMemberCall(); // original flags are not effected
    } else {
        TStack.Push(js_this_);
        SetThisGlobal();
    }

    if (TheFunction->IsNative()) {
        CallNative(F.O, Args);
        return false;
    }

    EndedConstructorCall();
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

    while (PSz > Sz) {
        V->StoreValue(Params[Sz++], CreateUndefinedObject());
    }

    // now we are in position to transfer the control
    // to the function
    Current = TheFunction->GetAddress();
    End = TheFunction->GetEnd();
    return true;
}

void VM::MarkCallOP()
{
    Flags |= member_call;
}

bool VM::IsMemberCall()
{
    return Flags & member_call;
}

void VM::EndMemberCall()
{
    Flags &= ~member_call;
}

void VM::CallOP()
{
    CallPrologue();
}

void VM::RetOP()
{
    // save the returned result
    auto ReturnedValue = Stack.Pop();
    
    auto V = GetVStore(Context);
    std::shared_ptr<Handle> This;
    // we check that whether we called a constructor
    // if yes then we will save the object created on to stack
    int32_t old_flags = FStack.Top();
    if (old_flags & constructor_call) {
        This = (GetThis());
        js_this_ = TStack.Pop();
    } else if (old_flags & member_call) {
        js_this_ = TStack.Pop();
    } else {
        js_this_ = TStack.Pop();
    }
    RestoreState();

    if (IsConstructorCall()) {
        Stack.Push(This);
        EndedConstructorCall();
    } else {
        // Save the return value
        Stack.Push(ReturnedValue);
    }
    if (IsMemberCall()) {
        EndMemberCall();
    }
    V->RemoveScope();
    SetFlags();
}

void VM::LeaveOP()
{
    Current = End;
}

void VM::PrintCurrentState()
{
    std::cout << InstructionToString(*GetCurrent()) << " ";
    std::cout << (Flags & zero_flag ? "Z " : " ") << std::endl;
}

void VM::ExecuteInstruction(std::shared_ptr<Instruction> &instr)
{
    I = instr;

    if (debug_execution_)
        PrintCurrentState();
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
    case Instructions::pop:
        PopOP();
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
    case Instructions::ltes:
        LtesOP();
        break;
    case Instructions::gtes:
        GtesOP();
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
    case Instructions::snot:
        SnotOP();
        break;
    case Instructions::bnot:
        BnotOP();
        break;
    case Instructions::pinc:
        PincOP();
        break;
    case Instructions::pdec:
        PdecOP();
        break;
    case Instructions::jmpnz:
        JmpnzOP();
        break;
    case Instructions::mem_call:
        MarkCallOP();
        break;
    }
}

void VM::HandleInterrupt()
{
    if (IsInterruptAcknowledging()) {
        return;
    }
    SetInterruptAcknowledge();
    // std::cout << "Handling interrupt: [ Count: " << RQ.Size() << "]"
    //     << " [ Flags: " << std::bitset<32>(Flags)
    //     << "] " << interrupt_flag << std::endl;

    size_t count = 0;
    // handle all interrupts in one go
    while (Interrupt()) {
        auto apair = RQ.PopFront();
        count++;
        SaveState();
        SetCounters(apair.first, apair.second);
        ClearInt();
        // std::cout << (Flags & interrupt_flag) << std::endl;
        Run();
        SetInt();
        RestoreState();
    }
    ClearAck();
}

void VM::Run()
{
    // main loop
    while (Current != End) {
        SetBusy();
        if (Interrupt()) {
            HandleInterrupt();
        }
        ExecuteInstruction(*Current);
        ++Current;
    }
    Flags &= ~is_running;
}

} // vm
} // grok

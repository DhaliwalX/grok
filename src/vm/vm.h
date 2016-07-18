#ifndef VM_H_
#define VM_H_

#include "vm/instruction-list.h"
#include "vm/vm_interrupts.h"
#include "vm/counter.h"
#include "vm/var-store.h"
#include "common/generic-stack.h"
#include "vm/context.h"
#include "grok/context.h"
#include "object/array.h"

#include <vector>
#include <type_traits>

namespace grok {
namespace vm {

using VMStack = GenericStack<Value>;
using CallStack = GenericStack<Counter>;
using FlagStack = GenericStack<int32_t>;
using PassedArguments = std::vector<Value>;
using ThisStack = GenericStack<std::shared_ptr<grok::obj::Object>>;
using VMStackHelper = GenericStack<VMStack::size_type>;
using ThisStackHelper = GenericStack<ThisStack::size_type>;

class VM;
extern std::unique_ptr<VM> CreateVM(VMContext *context);

///====---------------------------------------------------------------====
///                     VM ::= The Virtual Machine
///====---------------------------------------------------------------====
class VM {
#define DEFAULT_VM_FLAG (0 | 32)
    friend std::unique_ptr<VM> CreateVM(VMContext *context);

    VM()
        : Context{ nullptr }, AC{ }, Current{ }, End{ },
        Flags{ DEFAULT_VM_FLAG }, stack_level_{ 0 }, Stack{ }
    {
        debug_execution_ = grok::GetContext()->DebugExecution();
    }

public:
    enum {
        carry_flag = 1,
        zero_flag = 1 << 1,
        undefined_flag = 1 << 2,
        constructor_call = 1 << 3,
        member_call = 1 << 4,
        interrupt_flag = 1 << 5,
        interrupt_rq = 1 << 6,
        is_running = 1 << 7,
        interrupt_ack = 1 << 8,
        call_from_native = 1 << 9
    };

    /// SetContext ::= sets the context
    void SetContext(VMContext *context);

    /// GetResult ::= returns the result of the last executed
    /// statement in stack (if any)
    Value GetResult();

    /// SetCounters ::= Set program counters to start execution
    void SetCounters(Counter start, Counter end);

    /// Run ::= run the VM
    void Run();

    /// SaveState ::= saves the state i.e. flags and Counter
    void SaveState();

    void RestoreState();

    inline auto GetCurrent()
    {
        return I;
    }

    /// ExecuteInstruction ::= Execute the given instruction
    void ExecuteInstruction(std::shared_ptr<Instruction> &instr);

    /// GetThis ::= return the value of `this`
    std::shared_ptr<grok::obj::Object> GetThis();

    /// PushArg ::= push the argument to the stack
    void PushArg(std::shared_ptr<grok::obj::Object> Arg)
    {
        Stack.Push(Arg);
    }

    bool IsConstructorCall();
    void EndedConstructorCall();

    // Destroys the stacks explicitly
    void ShutDown();

    // Reset the counters, stacks etc.
    void Reset();

    void SetInt() { Flags |= interrupt_flag; }
    void ClearInt() { Flags &= ~interrupt_flag; }

    void SetIRQ() { Flags |= interrupt_rq; }
    void ClearIRQ() { Flags &= ~interrupt_rq; }

    void RequestInterrupt(Counter start, Counter end);

    bool Interrupt() { return !RQ.Empty() && (Flags & interrupt_flag); }
    void HandleInterrupt();

    bool IsRunning() { return Flags & is_running; }
    void SetBusy() { Flags |= is_running; }

    bool IsInterruptAcknowledging() { return Flags & interrupt_ack; }
    void SetInterruptAcknowledge() { Flags |= interrupt_ack; }
    void ClearAck() { Flags &= ~interrupt_ack; }
    void PrepareCallFromNative() { Flags |= call_from_native; }
    void EndCallFromNative() { Flags &= ~call_from_native; }

    void SetThis(std::shared_ptr<grok::obj::Handle>);
    void SetThisGlobal();
private:
    void IndexArray(std::shared_ptr<grok::obj::JSArray> arr,
        std::shared_ptr<grok::obj::Object> obj);
    void IndexObject(std::shared_ptr<grok::obj::JSObject> obj,
        std::shared_ptr<grok::obj::Object> idx);
    void PrintCurrentState();
    void SetAC(Value v);
    void NoOP();
    void FetchOP();
    void StoreOP();
    void PushNumber(double number);
    void PushString(const std::string &str);
    void PushNull();
    void PushBool(bool boolean);
    void PushOP();
    void PopOP();
    void PushimOP();
    void PoppropOP();
    void ReplpropOP();
    void IndexOP();
    void ResOP();
    void NewsOP();
    void CpyaOP();
    void MapsOP();
    void SetFlags();
    void AddsOP();
    void SubsOP();
    void MulsOP();
    void DivsOP();
    void RemsOP();
    void JmpOP();
    void JmpzOP();
    void JmpnzOP();
    PassedArguments CreateArgumentList(size_t sz);
    void CallOP();
    void RetOP();
    void LeaveOP();
    void GtsOP();
    void LtsOP();
    void GtesOP();
    void LtesOP();
    void EqsOP();
    void NeqsOP();
    void ShlsOP();
    void ShrsOP();
    void BorsOP();
    void BandsOP();
    void OrsOP();
    void AndsOP();
    void XorsOP();
    void MarkstOP();
    void PushthisOP();
    void IncOP();
    void DecOP();
    void SnotOP();
    void BnotOP();
    void PincOP();
    void PdecOP();
    void CallNative(std::shared_ptr<grok::obj::Handle> function,
            PassedArguments &Args);
    bool CallPrologue();
    void MarkCallOP();
    bool IsMemberCall();
    void EndMemberCall();

    bool debug_execution_;
    VMContext *Context;
    Value AC;  // accumulator
    std::shared_ptr<grok::obj::Handle> js_this_;
    std::shared_ptr<grok::obj::Handle> member_;
    Counter Current;  // current instruction being executed
    Counter Start;
    Counter End;  // end of the block
    ThisStack TStack; // `this` of javascript
    ThisStackHelper this_helper;
    VMStackHelper HelperStack;
    std::shared_ptr<Instruction> I; // current instruction

    int32_t Flags;  // flags for storing the VM state
    int32_t stack_level_;
    VMStack Stack;  // program stack
    CallStack CStack;
    FlagStack FStack;

    // RQ ::= runqueue
    IRQueue RQ;
};

} // vm
} // grok

#endif

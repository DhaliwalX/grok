#ifndef VM_H_
#define VM_H_

#include "vm/instruction-list.h"
#include "vm/var-store.h"
#include "common/generic-stack.h"
#include "vm/context.h"
#include "grok/context.h"
#include "object/array.h"

#include <vector>
#include <type_traits>

namespace grok {
namespace vm {

/// Counter ::= similar to program counter in processor
using Counter = InstructionList::iterator;
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
#define DEFAULT_VM_FLAG 0
    friend std::unique_ptr<VM> CreateVM(VMContext *context);

    VM()
        : Context{ nullptr }, AC{ }, Current{ }, End{ },
        Flags{ DEFAULT_VM_FLAG }, Stack{ }
    {
        debug_execution_ = grok::GetContext()->DebugExecution();
    }

public:
    enum {
        carry_flag = 0x1,
        zero_flag = 0x2,
        undefined_flag = 0x4,
        constructor_call = 0x8
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
    void ExecuteInstruction(std::shared_ptr<Instruction> instr);

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
    void CallNative(std::shared_ptr<grok::obj::Object> function,
            PassedArguments &Args);

    bool debug_execution_;
    VMContext *Context;
    Value AC;  // accumulator
    Counter Current;  // current instruction being executed
    Counter Start;
    Counter End;  // end of the block
    ThisStack TStack; // `this` of javascript
    ThisStackHelper this_helper;
    VMStackHelper HelperStack;
    std::shared_ptr<Instruction> I; // current instruction

    int32_t Flags;  // flags for storing the VM state
    VMStack Stack;  // program stack
    CallStack CStack;
    FlagStack FStack;
};

} // vm
} // grok

#endif

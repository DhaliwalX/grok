#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "object/jsobject.h"
#include "parser/functionstatement.h"
#include "object/argument.h"
#include "vm/instruction-list.h"
#include "vm/context.h"
#include "vm/counter.h"

#include <string>
#include <vector>

namespace grok {
namespace vm {
class VM;
}
}

namespace grok {
namespace obj {

/// NativeFunctionType ::= captures the type of the native function
using NativeFunctionType = std::shared_ptr<Object> (*)
                    (std::shared_ptr<grok::obj::Argument>);

/// Function ::= class to store the functions 
class Function : public JSObject {
public:
    Function();
    Function(std::shared_ptr<grok::parser::Expression> AST,
        std::shared_ptr<grok::parser::FunctionPrototype> proto);
    Function(NativeFunctionType function);
    ~Function() { };

    /// HasCodeGened ::= returns true if code for the function
    /// is already generated
    bool HasCodeGened() const;

    /// PrepareFunction ::= prepares the function
    /// do nothing if function is ready to execute i.e. code for the
    /// function is available else generated code for it and saves the
    /// code for future invocation
    void PrepareFunction();

    /// IsNative ::= returns true this function holds a native handler
    bool IsNative() const;

    /// GetAddress ::= returns the starting address of code
    grok::vm::Counter GetAddress();

    /// GetArgs ::= returns the argument vector
    const std::vector<std::string> &GetParams() const;

    grok::vm::Counter GetEnd()
    {
        return IR->end();
    }
    ObjectType GetType() const override
    { return ObjectType::_function; }

    std::string AsString() const override;

    std::string ToString() const override;

    void SetParams(std::vector<std::string> parms)
    {
        Params = parms;
    }

    virtual grok::vm::Value CallNative(std::vector<grok::vm::Value> Args,
        std::shared_ptr<JSObject> This);

protected:
    std::shared_ptr<grok::parser::Expression> AST;
    std::shared_ptr<grok::parser::FunctionPrototype> Proto;
    NativeFunctionType NFT;
    bool Native;
    bool CodeGened;     // for delayed code generation
    std::shared_ptr<grok::vm::InstructionList> IR;
    std::vector<std::string> Params;
};

// /// CreateFunction() ::= creates a function from args
static inline std::shared_ptr<Object>
CreateFunction(std::shared_ptr<grok::parser::Expression> AST,
    std::shared_ptr<grok::parser::FunctionPrototype> Proto)
{
    auto F = std::make_shared<Function>(AST, Proto);
    DefineInternalObjectProperties(F.get());
    return std::make_shared<Object>(F);
}

static inline std::shared_ptr<Object>
CreateFunction(NativeFunctionType NFT)
{
    auto F = std::make_shared<Function>(NFT);
    DefineInternalObjectProperties(F.get());
    return std::make_shared<Object>(F);
}

static inline bool
IsFunction(std::shared_ptr<Object> obj)
{
    return obj->as<JSObject>()->GetType() == ObjectType::_function;
}

extern std::shared_ptr<Object> CallJSFunction(std::shared_ptr<Function> func,
    std::shared_ptr<Argument> Args, grok::vm::VM* vm);

extern void CreateInterruptRequest(std::shared_ptr<Function> func,
        std::shared_ptr<Argument> Args, grok::vm::VM* vm);
} // obj
} // grok

#endif // FUNCTION_H_

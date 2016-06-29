#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "object/jsobject.h"
#include "object/argument.h"
#include "parser/functionstatement.h"
#include "vm/instruction-list.h"    // I don't like this dependency
#include "vm/vm.h"
#include "vm/var-store.h"

#include <string>
#include <vector>
#include <sstream>

namespace grok {
namespace obj {

/// NativeFunctionType ::= captures the type of the native function
using NativeFunctionType = std::shared_ptr<Object> (*)
                    (std::shared_ptr<grok::obj::Argument>);

/// Function ::= class to store the functions 
class Function : public JSObject {
public:
    Function()
        : JSObject{}, AST{}, Proto{ nullptr }, NFT{ nullptr },
          Native{ false }, CodeGened{ false }, IR{}, Params{ }
    { }

    Function(std::shared_ptr<grok::parser::Expression> AST,
        std::shared_ptr<grok::parser::FunctionPrototype> Proto)
        : JSObject(), AST{ AST }, Proto{ Proto }, NFT{ nullptr },
          Native{ false }, CodeGened{ false }, IR{}, Params{ Proto->GetArgs() }
    { }

    Function(NativeFunctionType function)
        : JSObject{}, AST{}, Proto{ nullptr }, NFT{ function },
          Native{ true }, CodeGened{ true }, IR{}, Params{}
    { }

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

    ObjectType GetType() const override
    { return ObjectType::_function; }

    std::string ToString() const override
    {
        std::ostringstream os;
        if (Native) {
            os << "function() { [native code] }";
            return os.str();
        }

        os << "function " << Proto->GetName();
        os << "(";
        auto Args = GetParams();
        for (auto Arg = Args.cbegin(); Arg != Args.cend() - 1; Arg++)
        {
            os << *Arg << ", ";
        }
        if (Args.size() > 0) {
            os << Args.back();
        }
        os << ") { [Code] }";
        return os.str();
    }

    void SetParams(std::vector<std::string> parms)
    {
        Params = parms;
    }

    grok::vm::Value CallNative(std::vector<grok::vm::Value> Args,
        std::shared_ptr<JSObject> This);
private:
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

} // obj
} // grok

#endif // FUNCTION_H_

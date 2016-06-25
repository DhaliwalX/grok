#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "object/jsobject.h"
#include "parser/functionstatement.h"
#include "vm/instruction-list.h"    // I don't like this dependency
#include "vm/vm.h"

#include <string>
#include <sstream>

namespace grok {
namespace obj {

/// NativeFunctionType ::= captures the type of the native function
using NativeFunctionType = void (*)(void);

/// Function ::= class to store the functions 
class Function : public JSBasicObject {
public:
    Function()
        : JSBasicObject{}, AST{}, Proto{ nullptr }, NFT{ nullptr },
          Native{ false }, CodeGened{ false }, IR{}
    { }

    Function(std::shared_ptr<grok::parser::Expression> AST,
        std::shared_ptr<grok::parser::FunctionPrototype> Proto)
        : JSBasicObject(), AST{ AST }, Proto{ Proto }, NFT{ nullptr },
          Native{ false }, CodeGened{ false }, IR{}
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
    const std::vector<std::string> &GetArgs() const;

    ObjectType GetType() const override
    { return ObjectType::_function; }

    std::string ToString() const override
    {
        std::ostringstream os;

        os << "function " << Proto->GetName();
        os << "(";
        auto Args = GetArgs();
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

private:
    std::shared_ptr<grok::parser::Expression> AST;
    std::shared_ptr<grok::parser::FunctionPrototype> Proto;
    NativeFunctionType NFT;
    bool Native;
    bool CodeGened;     // for delayed code generation
    std::shared_ptr<grok::vm::InstructionList> IR;
};

// /// CreateFunction() ::= creates a function from args
// static inline std::shared_ptr<Function>
// CreateFunction(std::shared_ptr<grok::parser::Expression> AST,
//     std::shared_ptr<grok::parser::FunctionPrototype> Proto)
// {
//     auto F = std::make_shared<Function>(AST, Proto);
//     return F;
// }

} // obj
} // grok

#endif // FUNCTION_H_

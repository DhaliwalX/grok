#include "object/array.h"
#include "libs/array/array_constructor.h"
#include "object/function.h"
#include "vm/vm.h"

namespace grok {
namespace libs {

using namespace grok::obj;

std::shared_ptr<Object> ArrayConstructor(std::shared_ptr<Argument> Args)
{
    // we will execute the code only if we are called as constructor call
    // we will ask VM for that
    grok::vm::VM *vm = grok::vm::GetGlobalVMContext()->GetVM();

    if (!vm->IsConstructorCall()) {
        return CreateUndefinedObject();
    }

    auto This = Args->GetProperty("this");
    if (Args->Size() == 0) {
        return CreateArray(0);
    } else if (Args->Size() == 1) {
        auto sz = Args->At(0);

        if (IsJSNumber(sz)) {
            auto num = sz->as<JSDouble>()->GetNumber();
            return CreateArray(num);
        } else {
            auto Arr = CreateArray(0);
            auto A = Arr->as<JSArray>();
            for (auto Arg : *Args) {
                A->Push(Arg);
            }
            return Arr;
        }
    } else {
        auto Arr = CreateArray(0);
        auto A = Arr->as<JSArray>();
        for (auto Arg : *Args) {
            A->Push(Arg);
        }
        return Arr;
    }

    return CreateUndefinedObject();
}

std::shared_ptr<Object> CreateArrayConstructorObject()
{
    std::shared_ptr<Object> wrapped_function = CreateFunction(ArrayConstructor);

    auto ctor = wrapped_function->as<Function>();

    ctor->AddProperty("prototype", JSArray::array_handle);
    return wrapped_function;
}

}
}

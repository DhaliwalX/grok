#include "libs/array-map.h"
#include "object/array.h"
#include "object/function.h"
#include "vm/context.h"

namespace grok {
namespace libs {

using namespace grok::vm;
using namespace grok::obj;

std::shared_ptr<Object> ArrayMap(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateArray(0);
    auto A = This->as<JSArray>();
    auto Result = CreateArray(0);
    auto Func = Args->GetProperty("callback");

    if (!IsFunction(Func))
        throw std::runtime_error("TypeError: callback provided is not a "
            "function");

    auto F = Func->as<Function>();
    auto vm = GetGlobalVMContext()->GetVM();

    auto RA = Result->as<JSArray>();
    size_t count = 0;
    for (auto i : *A) {
        auto args_to_pass = CreateArgumentObject()->as<Argument>();
        args_to_pass->Push(i);
        args_to_pass->Push(CreateJSNumber(count++));

        auto R = CallJSFunction(F, args_to_pass, vm);
        RA->Push(R);
    }
    return (Result);
}

}
}

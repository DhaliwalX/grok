#include "libs/array-concat.h"

#include "object/array.h"

namespace grok {
namespace libs {
using namespace grok::obj;

void ConcatSingleArg(JSArray *arr, std::shared_ptr<Object> obj)
{
    if (IsJSArray(obj)) {
        auto A = obj->as<JSArray>();

        for (auto i : *A) {
            arr->Push(i);
        }
        return;
    }
    // else just append the element
    arr->Push(obj);
}

std::shared_ptr<Object> ArrayConcat(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");
    auto Result = CreateArray(0)->as<JSArray>();

    if (IsJSArray(This)) {
        ConcatSingleArg(Result.get(), This);
    }

    // concat every arg
    for (auto Arg : *Args) {
        ConcatSingleArg(Result.get(), Arg);
    }

    return std::make_shared<Object>(Result);
}

} // libs
} // grok

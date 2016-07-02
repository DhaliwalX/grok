#include "libs/array/push-pop.h"
#include "object/array.h"

namespace grok {
namespace libs {

using namespace grok::obj;

std::shared_ptr<Object> ArrayPush(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto Arr = This->as<JSArray>();

    for (auto &Arg : *Args) {
        Arr->Push(Arg);
    }

    auto Sz = Arr->Size();

    return CreateJSNumber(Sz);
}

std::shared_ptr<Object> ArrayPop(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto Arr = This->as<JSArray>();

    auto element = Arr->Pop();

    return element;
}

} // libs
} // grok

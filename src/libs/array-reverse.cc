#include "libs/array-reverse.h"
#include "object/array.h"
#include <algorithm>

namespace grok {
namespace libs {

using namespace grok::obj;

std::shared_ptr<Object> ArrayReverse(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto A = This->as<JSArray>();
    auto Beg = A->begin();
    auto End = --A->end();

    while (Beg < End) {
        std::swap(*Beg, *End);
        Beg++;
        End--;
    }

    return This;
}

} // libs
} // grok

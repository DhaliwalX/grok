#include "libs/array-shift-unshift.h"
#include "object/array.h"

#include <algorithm>

namespace grok {
namespace libs {

using namespace grok::obj;

std::shared_ptr<Object> ArrayShift(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto A = This->as<JSArray>();

    if (A->Size() == 0)
        return CreateUndefinedObject();
    auto First = *A->begin();

    // shift all elements
    std::copy(A->begin() + 1, A->end(), A->begin());
    A->Resize(A->Size() - 1);
    return First;
}

std::shared_ptr<Object> ArrayUnshift(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto A = This->as<JSArray>();
    auto &C = A->Container();

    C.insert(C.begin(), Args->begin(), Args->end());
    return CreateJSNumber(C.size());
}

}
}

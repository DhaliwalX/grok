#include "libs/array-slice.h"
#include "object/array.h"

#include <iostream>
namespace grok {
namespace libs {
using namespace grok::obj;

auto GetIndex(JSArray::size_type sz, std::shared_ptr<Object> obj,
    JSArray::size_type def) 
                -> decltype(sz)
{
    std::string str;
    if (!IsJSNumber(obj)) {
        str = obj->as<JSObject>()->ToString();
        obj = CreateJSNumber(str);

        if (IsUndefined(obj))
            return def;
    }

    auto num = obj->as<JSNumber>()->GetNumber();
    if (num < 0)
        num += sz;

    if (num < 0)
        return 0;
    if ((num) > ((decltype(num))sz) - 1) {
        num = sz - 1;
    }
    return num;
}

std::shared_ptr<Object> ArraySlice(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSArray(This))
        return CreateUndefinedObject();

    auto A = This->as<JSArray>();
    auto Start = Args->GetProperty("start");
    auto End = Args->GetProperty("end");
    auto Sz = A->Size();

    auto start_idx = GetIndex(Sz, Start, 0);
    auto end_idx = GetIndex(Sz, End, Sz - 1);

    if (start_idx > end_idx)
        return CreateArray(0);

    std::cout << start_idx  << ", " << end_idx << std::endl;
    auto result = CreateArray(end_idx - start_idx + 1);
    auto result_array = result->as<JSArray>();
    std::copy(A->begin() + start_idx, A->begin() + end_idx + 1,
        result_array->begin());
    return result;
}

}
}

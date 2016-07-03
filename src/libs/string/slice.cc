#include "libs/string/slice.h"
#include "object/jsobject.h"

#include <iostream>
namespace grok {
namespace libs {
using namespace grok::obj;

auto GetStringIndex(JSString::size_type sz, std::shared_ptr<Object> obj,
    JSString::size_type def) 
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
    if ((num) > ((decltype(num))sz)) {
        num = sz;
    }
    return num;
}

std::shared_ptr<Object> StringSlice(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    if (!IsJSString(This))
        return CreateUndefinedObject();

    auto A = This->as<JSString>();
    auto Astr = A->ToString();
    auto Start = Args->GetProperty("start");
    auto End = Args->GetProperty("end");
    auto Sz = Astr.size();

    auto start_idx = GetStringIndex(Sz, Start, 0);
    auto end_idx = GetStringIndex(Sz, End, Sz - 1);

    if (start_idx > end_idx)
        return CreateJSString("");

    auto result = CreateJSString("");
    auto &result_str = result->as<JSString>()->GetString();
    result_str.assign(Astr.begin() + start_idx, Astr.begin() + end_idx + 1);
    return result;
}

}
}

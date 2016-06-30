#include "libs/array-join.h"
#include "object/array.h"

namespace grok {
namespace libs {

using namespace grok::obj;

std::string JoinJSArray(std::shared_ptr<JSArray> Arr, std::string Sep)
{
    std::string res;
    if (Arr->Size() == 0)
        return res;
    for (auto it = Arr->begin(); it != Arr->end() - 1; ++it) {
        auto obj = (*it)->as<JSObject>();
        res += obj->ToString();
        res += Sep;
    }

    res += (*(Arr->end() - 1))->as<JSObject>()->ToString();
    return res;
}

std::shared_ptr<Object> ArrayJoin(std::shared_ptr<Argument> Args)
{
    // get the array on which we are operating
    auto A = Args->GetProperty("this");

    if (!IsJSArray(A))
        return CreateUndefinedObject();

    auto Arr = A->as<JSArray>();
    // get the separator, default is ','
    auto Separator = Args->GetProperty("sep");

    std::string sepstr;
    if (IsUndefined(Separator))
        sepstr = ",";
    else
        sepstr = Separator->as<JSObject>()->ToString();

    auto Joined = JoinJSArray(Arr, sepstr);

    return CreateJSString(Joined);
}

} // libs
} // grok

#include "libs/array/sort.h"
#include "object/array.h"
#include "object/argument.h"
#include "object/function.h"

#include <algorithm>

namespace grok {
namespace libs {
using namespace grok::vm;
using namespace grok::obj;

bool MyPred(std::shared_ptr<Object> A, std::shared_ptr<Object> B)
{
    auto strA = A->as<JSObject>()->ToString();
    auto strB = B->as<JSObject>()->ToString();

    return strA < strB;
}

void SortArrayWithDefaultPredicate(std::shared_ptr<JSArray> arr)
{
    std::sort(arr->begin(), arr->end(), MyPred);
}

void SortStringWithDefaultPredicate(std::shared_ptr<JSString> str)
{
    auto &S = str->GetString();
    std::sort(S.begin(), S.end());
}

void SortWithDefaultPredicate(std::shared_ptr<Object> A)
{
    if (IsJSArray(A)) {
        SortArrayWithDefaultPredicate(A->as<JSArray>());
    } else if (IsJSString(A)) {
        SortStringWithDefaultPredicate(A->as<JSString>());
    } else {
        return;
    }
}

void SortWithPredicate(std::shared_ptr<Object> obj,
        std::shared_ptr<Function> func)
{
    if (!IsJSArray(obj))
        return;

    auto A = obj->as<JSArray>();
    std::sort(A->begin(), A->end(),
            [&func](auto a, auto b) {
                auto Args = CreateArgumentObject()->as<Argument>();
                Args->Push(a);
                Args->Push(b);
                auto vm = GetGlobalVMContext()->GetVM();

                auto R = CallJSFunction(func, Args, vm);

                int n = 0;
                if (IsJSNumber(R)) {
                    n = (int)R->as<JSNumber>()->GetNumber();
                    return n < 0 ? true : false;
                }
                return false;
            });
}

void SortInternal(std::shared_ptr<Object> A,
    std::shared_ptr<Object> pred, bool use_default_pred)
{
    if (use_default_pred) {
        SortWithDefaultPredicate(A);
    } else {
        if (!IsFunction(pred))
            SortWithDefaultPredicate(A);
        else 
            SortWithPredicate(A, pred->as<Function>());
    }
}

// this is a very generic sort function and can be applied to strings
// and arrays or any other object having property length
std::shared_ptr<Object> ArraySort(std::shared_ptr<Argument> Args)
{
    bool use_default_pred = false;

    // object on which we are applying sort
    auto Obj = Args->GetProperty("this");
    auto O = Obj->as<JSObject>();

    // predicate
    auto Pred = Args->GetProperty("pred");

    if (IsUndefined(Pred)) {
        use_default_pred = true;
    }

    SortInternal(Obj, Pred, use_default_pred);
    return Obj;
}


}
} // grok

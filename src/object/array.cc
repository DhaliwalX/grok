#include "object/array.h"
#include "object/argument.h"
#include "object/function.h"

#include <algorithm>

namespace grok {
namespace obj {

std::shared_ptr<Object> ArrayConstructor(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");
    if (Args->Size() == 0) {
        This->Reset(CreateArray(0));
    } else if (Args->Size() >= 1) {
        auto sz = Args->At(0);

        if (IsJSNumber(sz)) {
            auto num = sz->as<JSNumber>()->GetNumber();
            This->Reset(CreateArray(num));
        }
        auto Arr = CreateArray(0);
        auto A = Arr->as<JSArray>();
        for (auto Arg : *Args) {
            A->Push(Arg);
        }
        This->Reset(Arr);
    }
    return CreateUndefinedObject();
}

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

void SortInternal(std::shared_ptr<Object> A,
    std::shared_ptr<Object> pred, bool use_default_pred)
{
    if (use_default_pred) {
        SortWithDefaultPredicate(A);
    }
    (void)pred;
}

// this is a very generic sort function and can be applied to strings
// and arrays or any other object having property length
std::shared_ptr<Object> Sort(std::shared_ptr<Argument> Args)
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

JSObject::Value JSArray::GetProperty(const JSObject::Name &name)
{
    if (name == "length") {
        return CreateJSNumber(Size());
    }
    return JSObject::GetProperty(name);
}

std::shared_ptr<Object> CreateArray(size_t size)
{
    auto ptr = std::make_shared<JSArray>();
    ptr->Resize(size);
    DefineInternalObjectProperties(ptr.get());

    auto S = std::make_shared<Function>(Sort);
    S->SetNonWritable();
    S->SetNonEnumerable();
    S->SetParams({ "pred" });
    ptr->AddProperty(std::string("sort"), std::make_shared<Object>(S));
    auto Wrap = std::make_shared<Object>(ptr);
    return Wrap;
}

}
}

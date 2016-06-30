#include "object/array.h"
#include "object/argument.h"
#include "object/function.h"

#include "libs/array-sort.h" // ArraySort
#include "libs/array-join.h" // ArrayJoin

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

    // a.sort()
    auto S = std::make_shared<Function>(grok::libs::ArraySort);
    S->SetNonWritable();
    S->SetNonEnumerable();
    S->SetParams({ "pred" });
    ptr->AddProperty(std::string("sort"), std::make_shared<Object>(S));

    // a.join()
    S = std::make_shared<Function>(grok::libs::ArrayJoin);
    S->SetNonWritable();
    S->SetNonEnumerable();
    S->SetParams({ "sep" });
    ptr->AddProperty(std::string("join"), std::make_shared<Object>(S));
    

    auto Wrap = std::make_shared<Object>(ptr);
    return Wrap;
}

}
}

#include "object/array.h"
#include "object/argument.h"
#include "object/function.h"

#include "libs/array/sort.h" // ArraySort
#include "libs/array/join.h" // ArrayJoin
#include "libs/array/push-pop.h"
#include "libs/array/reverse.h"
#include "libs/array/concat.h"
#include "libs/array/shift-unshift.h"
#include "libs/array/map.h"
#include "libs/array/slice.h"

namespace grok {
namespace obj {

std::string JSArray::AsString() const
{
    std::string buff = "[ ";
    for (const auto &element : elements_) {
        buff += element->as<JSObject>()->AsString();
        buff += ", ";
    }
    if (elements_.size()) {
        buff.pop_back();
        buff.pop_back();
    }
    return buff + " ]";
}

std::string JSArray::ToString() const
{
    std::string buff = "";
    for (const auto &element : elements_) {
        buff += element->as<JSObject>()->ToString();
        buff += ",";
    }
    if (elements_.size()) {
        buff.pop_back();
    }
    return buff;
}

JSObject::Value JSArray::GetProperty(const JSObject::Name &name)
{
    if (name == "length") {
        return CreateJSNumber(Size());
    }
    return At(name);
}

JSArray::HandlePointer JSArray::At(const std::string &prop)
{
    auto p = GetStaticProperty(prop);

    if (p.second) {
        return p.first;
    }

    size_type idx = 0;
    // we still try to convert the string into number
    try {
        idx = std::stod(prop);
    } catch (...) {
        return JSObject::GetProperty(prop);
    }
    return this->At(idx);
}

std::shared_ptr<Handle> JSArray::array_handle;

std::pair<std::shared_ptr<Handle>, bool>
 JSArray::GetStaticProperty(const std::string &str)
{
    auto arr = array_handle->as<Function>();

    if (!arr->HasProperty(str))
        return { nullptr, false };
    auto prop = arr->JSObject::GetProperty(str);
    return { prop, true };
}

std::shared_ptr<Object> CreateArray(size_t size)
{
    auto ptr = std::make_shared<JSArray>();
    ptr->Resize(size);
    for (size_t i = 0; i < size; i++) {
        ptr->Assign(i, CreateUndefinedObject());
    }
    return std::make_shared<Handle>(ptr);
}

void JSArray::Init()
{
    array_handle = CreateArray(0);
    auto ptr = array_handle->as<JSArray>();
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

    // a.push
    S = std::make_shared<Function>(grok::libs::ArrayPush);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("push"), std::make_shared<Object>(S));

    // a.pop
    S = std::make_shared<Function>(grok::libs::ArrayPop);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("pop"), std::make_shared<Object>(S));

    // a.reverse
    S = std::make_shared<Function>(grok::libs::ArrayReverse);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("reverse"), std::make_shared<Object>(S));

    // a.concat
    S = std::make_shared<Function>(grok::libs::ArrayConcat);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("concat"), std::make_shared<Object>(S));

    // a.shift
    S = std::make_shared<Function>(grok::libs::ArrayShift);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("shift"), std::make_shared<Object>(S));

    // a.unshift
    S = std::make_shared<Function>(grok::libs::ArrayUnshift);
    S->SetNonWritable();
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("unshift"), std::make_shared<Object>(S));

    // a.map
    S = std::make_shared<Function>(grok::libs::ArrayMap);
    S->SetNonWritable();
    S->SetParams({ "callback" });
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("map"), std::make_shared<Object>(S));

    // a.slice
    S = std::make_shared<Function>(grok::libs::ArraySlice);
    S->SetNonWritable();
    S->SetParams({ "start", "end" });
    S->SetNonEnumerable();
    ptr->AddProperty(std::string("slice"), std::make_shared<Object>(S));
}

}
}

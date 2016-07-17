#include "object/jsbasicobject.h"
#include "object/function.h"
#include "object/argument.h"
#include "object/jsobject.h"

namespace grok {
namespace obj {

std::string JSObject::AsString() const
{
    std::string buff = "";
    buff += "{\n";
    for (const auto &it : object_) {
        auto prop = it.second->as<JSObject>();
        if (!prop->IsEnumerable())
            continue;
        buff += it.first + ": ";
        if (it.second->as<JSObject>().get() == this) {
            buff += "[ Circular ],\n";
            continue;
        }
        buff += it.second->as<JSObject>()->AsString() += ",\n";
    }
    buff.pop_back(); // " "
    buff += " }\n";
    return buff;
}

std::string JSObject::ToString() const
{
    return "[ object Object ]";
}

std::shared_ptr<Object> toString(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    auto result = CreateJSString(This->as<JSObject>()->ToString());
    return result;
}

std::shared_ptr<Object> hasOwnProperty(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");

    auto Prop = Args->GetProperty("prop");
    auto Name = Prop->as<JSObject>()->ToString();
    // not according to ECMAScript standard
    return CreateJSNumber(This->as<JSObject>()->HasProperty(Name));
}

void DefineInternalObjectProperties(JSObject *obj)
{
    auto F = std::make_shared<Function>(toString);
    F->SetNonWritable();
    F->SetNonEnumerable();
    obj->AddProperty(std::string("toString"), std::make_shared<Object>(F));

    F = std::make_shared<Function>(hasOwnProperty);
    F->SetNonWritable();
    F->SetNonEnumerable();
    F->SetParams({ "prop" });
    obj->AddProperty(std::string("hasOwnProperty"),
        std::make_shared<Object>(F));

    // add the prototype property default is undefined
    auto P = std::make_shared<JSObject>();
    F->SetNonEnumerable();
    obj->AddProperty(std::string("prototype"), std::make_shared<Object>(P));
}

std::shared_ptr<Object> CreateJSObject()
{
    auto O = std::make_shared<JSObject>();
    DefineInternalObjectProperties(O.get());
    return std::make_shared<Object>(O);
}

std::shared_ptr<Object> ObjectConstructor(std::shared_ptr<Argument> Args)
{
    if (Args->Size() < 1) {
        return CreateJSObject();
    }
    return Args->At(0);
}

std::shared_ptr<Object> CreateCopy(std::shared_ptr<Object> obj)
{
    if (IsJSNumber(obj)) {
        return CreateJSNumber(obj->as<JSDouble>()->GetNumber());
    } else if (IsJSString(obj)) {
        return CreateJSString(obj->as<JSString>()->ToString());
    }
    // else return reference
    return obj;
}

} // obj
} // grok

#include "object/jsbasicobject.h"
#include "object/function.h"
#include "object/argument.h"
#include "object/jsobject.h"

namespace grok {
namespace obj {

std::string JSObject::ToString() const
{
    std::string buff = "";
    buff += "{ ";
    for (const auto &it : object_) {
        auto prop = it.second->as<JSObject>();
        if (!prop->IsEnumerable())
            continue;
        buff += it.first + ": ";
        buff += it.second->as<JSObject>()->ToString() += ", ";
    }
    buff.pop_back(); // " "
    buff += " }";
    return buff;
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

} // obj
} // grok

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

std::shared_ptr<Object> ObjectConstructor(std::shared_ptr<Argument> Args)
{
    if (Args->Size() < 1) {
        return CreateJSObject();
    }
    return Args->At(0);
}

std::shared_ptr<Handle> JSObject::st_obj;

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

void JSObject::Init()
{
    st_obj = CreateJSObject();
    auto obj = st_obj->as<JSObject>();

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

    auto o = std::make_shared<JSObject>();
    obj->AddProperty("prototype", std::make_shared<Handle>(o));

    F = std::make_shared<Function>(ObjectConstructor);
    obj->AddProperty("constructor", std::make_shared<Handle>(F));
}

std::pair<std::shared_ptr<Handle>, bool>
 CheckForInheritedProperty(const std::string &name,
        std::shared_ptr<Handle> ctor_handle)
{
    auto ctor = ctor_handle->as<JSObject>();
    auto proto_handle = ctor->GetProperty("prototype");
    auto proto = proto_handle->as<JSObject>();

    // find the property in the prototype object
    if (!proto->HasProperty(name)) {
        return { nullptr, false };
    }

    return { proto->GetProperty(name), true };
}

std::shared_ptr<Handle> JSObject::GetProperty(const std::string &name)
{
    // if the object has its own custom property, then go for it
    if (HasProperty(name)) {
        return object_.find(name)->second;
    }

    auto ctor_it = object_.find("constructor");

    std::pair<std::shared_ptr<Handle>, bool> p;

    if (name == "constructor" && ctor_it != object_.end()) {
        return ctor_it->second;
    }
    // check for inherited property
    if (ctor_it != object_.end()) {
        p = CheckForInheritedProperty(name, ctor_it->second);
    }
    if (!p.second) {
        // check for built-in property
        p = GetStaticProperty(name);
    }
    if (p.second) {
        return p.first;
    }
    auto value = object_.find(name);
    if ((value) != object_.end()) {
        return value->second;
    }
    auto def = CreateUndefinedObject();
    AddProperty(name, def); // an ugly hack to add properties at runtime
    return def;
}

std::pair<std::shared_ptr<Handle>, bool>
 JSObject::GetStaticProperty(const std::string &name)
{
    auto st = st_obj->as<JSObject>();

    if (!st->HasProperty(name)) {
        return { nullptr, false };
    }

    return { st->GetProperty(name), true };
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

    auto o = std::make_shared<JSObject>();
    obj->AddProperty("prototype", std::make_shared<Handle>(o));
}

std::shared_ptr<Object> CreateJSObject()
{
    auto O = std::make_shared<JSObject>();
    return std::make_shared<Object>(O);
}

std::shared_ptr<Object> CreateCopy(std::shared_ptr<Object> obj)
{
    if (IsJSNumber(obj)) {
        if (obj->as<JSObject>()->GetType() == ObjectType::_number)
            return CreateJSNumber(obj->as<JSNumber>()->GetNumber());
        return CreateJSNumber(obj->as<JSDouble>()->GetNumber());
    } else if (IsJSString(obj)) {
        return CreateJSString(obj->as<JSString>()->ToString());
    }
    // else return reference
    return obj;
}

} // obj
} // grok

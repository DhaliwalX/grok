#include "object/jsstring.h"

#include "libs/string/properties.h"
#include "common/colors.h"

namespace grok {
namespace obj {

JSString JSString::string;

JSObject::Value JSString::GetProperty(const std::string &prop)
{
    double idx = 0;
    try {
        idx = std::stod(prop);
    } catch (...) {
        if (prop == "length") {
            return CreateJSNumber(js_string_.size());
        }
        auto method = GetStaticProperty(prop);
        if (method.second) {
            return method.first;
        }
        return JSObject::GetProperty(prop);
    }

    return CreateJSString(std::string() + js_string_[idx]);
}

std::string JSString::AsString() const
{
    return Color::Color(Color::fgreen) + std::string("'") + js_string_
            + "'" + Color::Reset();
}

std::pair<std::shared_ptr<Handle>, bool>
JSString::GetStaticProperty(const std::string &str)
{
    if (!string.HasProperty(str))
        return { nullptr, false };

    auto prop = string.JSObject::GetProperty(str);
    return { prop, true };
} 

void JSString::Init()
{
    grok::libs::DefineStringProperties(&string);
}

void JSString::concat(const std::string &str)
{
    js_string_ += str;
}

std::shared_ptr<Object> CreateJSString(std::string str)
{
    auto S = std::make_shared<JSString>(str);
    auto W = std::make_shared<Object>(S);
    DefineInternalObjectProperties(S.get());
    return W;
}

}
}


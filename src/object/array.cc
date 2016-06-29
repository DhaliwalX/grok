#include "object/array.h"

namespace grok {
namespace obj {

std::shared_ptr<Object> CreateArray(size_t size)
{
    auto ptr = std::make_shared<JSArray>();
    ptr->Resize(size);
    DefineInternalObjectProperties(ptr.get());
    auto Wrap = std::make_shared<Object>(ptr);
    return Wrap;
}

JSObject::Value JSArray::GetProperty(const JSObject::Name &name)
{
    if (name == "length") {
        return CreateJSNumber(Size());
    }
    return JSObject::GetProperty(name);
}

}
}

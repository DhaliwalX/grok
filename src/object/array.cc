#include "object/array.h"

namespace grok {
namespace obj {

std::shared_ptr<Object> CreateArray(size_t size)
{
    auto ptr = std::make_shared<JSArray>();
    ptr->Resize(size);
    auto Wrap = std::make_shared<Object>(ptr);

    return Wrap;
}

}
}

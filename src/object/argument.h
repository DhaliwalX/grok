#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include "object/array.h"

namespace grok {
namespace obj {

class Argument : public JSArray {
public:
    Argument() :
        JSArray()
    { }
};

static inline auto CreateArgumentObject()
{
    auto arg = std::make_shared<Argument>();
    auto O = std::make_shared<Object>(arg);
    return O;
}

} // obj
} // grok

#endif // argument.h

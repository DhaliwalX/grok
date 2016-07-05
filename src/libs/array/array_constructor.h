#ifndef ARRAY_CONSTRUCTOR_H_
#define ARRAY_CONSTRUCTOR_H_

#include "object/argument.h"

namespace grok {
namespace libs {

extern std::shared_ptr<grok::obj::Object>
    ArrayConstructor(std::shared_ptr<grok::obj::Argument> Args);

extern std::shared_ptr<grok::obj::Object> CreateArrayConstructorObject();

}
}

#endif

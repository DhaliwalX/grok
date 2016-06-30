#ifndef ARRAY_PUSH_POP_H_
#define ARRAY_PUSH_POP_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArrayPush ::= push the element to the last position
extern std::shared_ptr<grok::obj::Object>
ArrayPush(std::shared_ptr<grok::obj::Argument> Args);

/// ArrayPop ::= pop a element from the array
extern std::shared_ptr<grok::obj::Object>
ArrayPop(std::shared_ptr<grok::obj::Argument> Args);

}
}

#endif // array-stack-operations.h

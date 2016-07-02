#ifndef ARRAY_JOIN_H_
#define ARRAY_JOIN_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArrayJoin ::= join the elements of an array
extern std::shared_ptr<grok::obj::Object>
ArrayJoin(std::shared_ptr<grok::obj::Argument> Args);

}
}

#endif // array-join.h

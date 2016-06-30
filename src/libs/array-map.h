#ifndef ARRAY_MAP_H_
#define ARRAY_MAP_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArrayMap ::= The map() method creates a new array with the results
/// of calling a provided function on every element in this array.
extern std::shared_ptr<grok::obj::Object>
ArrayMap(std::shared_ptr<grok::obj::Argument> Args);

}
}

#endif

#ifndef ARRAY_SLICE_H_
#define ARRAY_SLICE_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// The slice() method returns a shallow copy of a portion of an array
/// into a new array object.
extern std::shared_ptr<grok::obj::Object>
ArraySlice(std::shared_ptr<grok::obj::Argument> Args);

} // grok
} // libs

#endif

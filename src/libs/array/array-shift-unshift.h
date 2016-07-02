#ifndef ARRAY_SHIFT_UNSHIFT_H_
#define ARRAY_SHIFT_UNSHIFT_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArrayShift ::= The shift() method removes the first element from an
/// array and returns that element. This method changes the length of the array.
extern std::shared_ptr<grok::obj::Object>
ArrayShift(std::shared_ptr<grok::obj::Argument> Args);

/// ArrayUnshift ::= The unshift() method adds one or more elements to the
/// beginning of an array and returns the new length of the array.
extern std::shared_ptr<grok::obj::Object>
ArrayUnshift(std::shared_ptr<grok::obj::Argument> Args);

} // libs
} // grok

#endif

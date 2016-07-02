#ifndef ARRAY_SORT_H_
#define ARRAY_SORT_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArraySort ::= sorts a JS array, default property of every array
extern std::shared_ptr<grok::obj::Object>
ArraySort(std::shared_ptr<grok::obj::Argument>);

} // libs
} // grok

#endif // array-sort.h

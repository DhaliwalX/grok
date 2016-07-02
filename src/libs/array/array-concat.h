#ifndef ARRAY_CONCAT_H_
#define ARRAY_CONCAT_H_

#include "object/argument.h"

namespace grok {
namespace libs {

/// ArrayConcat ::= concat creates a new array consisting of the 
/// elements in the object on which it is called, followed in order by,
/// for each argument, the elements of that argument (if the argument is
/// an array) or the argument itself (if the argument is not an array).
extern std::shared_ptr<grok::obj::Object>
ArrayConcat(std::shared_ptr<grok::obj::Argument> Args);

}
}

#endif

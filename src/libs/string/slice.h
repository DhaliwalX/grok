#ifndef STRING_SLICE_H_
#define STRING_SLICE_H_

#include "object/argument.h"

namespace grok {
namespace libs {

std::shared_ptr<grok::obj::Object>
StringSlice(std::shared_ptr<grok::obj::Argument> Args);

}
}

#endif

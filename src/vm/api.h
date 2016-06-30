#ifndef VM_API_H_
#define VM_API_H_

#include "object/jsbasicobject.h"
#include "object/jsobject.h"
#include "object/argument.h"

namespace grok {
namespace vm {

using GrokFunction = Value (*) (std::shared_ptr<grok::obj::Argument>);

}
}

#endif

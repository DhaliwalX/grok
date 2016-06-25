#ifndef VM_H_
#define VM_H_

#include "vm/instruction-list.h"

namespace grok {
namespace vm {

/// Counter ::= similar to program counter in processor
using Counter = InstructionList::iterator;

} // vm
} // grok

#endif

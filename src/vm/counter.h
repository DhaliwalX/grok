#ifndef COUNTER_H_
#define COUNTER_H_

#include "vm/instruction-list.h"

namespace grok {
namespace vm {

/// Counter ::= similar to program counter in processor
using Counter = InstructionList::iterator;

}
}

#endif

#ifndef INSTRUCTION_VISITOR_H_
#define INSTRUCTION_VISITOR_H_

#include "vm/instruction.h"
#include "vm/instructions.h"

namespace grok {
namespace vm {

class InstructionVisitor {
public:
#define DECLARE_VIRTUAL_METHOD(Instr, Class)    \
    virtual void Visit(Class##Instruction *instr) = 0;
INSTRUCTION_LIST_FOR_EACH(DECLARE_VIRTUAL_METHOD)
#undef DECLARE_VIRTUAL_METHOD
};

}
}

#endif

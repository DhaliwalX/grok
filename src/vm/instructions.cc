#include "vm/instructions.h"
#include "vm/instruction.h"
#include "vm/instruction-visitor.h"

namespace grok {
namespace vm {

#define DEFINE_VISITOR_METHOD(Kind, Class)  \
    void Class ## Instruction::AcceptVisitor(InstructionVisitor *v) \
    {   \
        v->Visit(this); \
    }

INSTRUCTION_LIST_FOR_EACH(DEFINE_VISITOR_METHOD)

}
}

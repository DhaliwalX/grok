#ifndef PRINTER_H_
#define PRINTER_H_

#include "vm/instruction.h"
#include "vm/instruction-visitor.h"
#include "vm/instruction-list.h"
#include "common/util.h"

#include <iostream>

namespace grok {
namespace vm {

/// Printer ::= a utility class to print the code
class Printer {
    DELETE_COPY_AND_MOVE_CONSTRUCTORS(Printer)
public:
    Printer(std::ostream &os) 
        : os{os}
    { }

    /// Print ::= print the code to stream `os`
    void Print(std::shared_ptr<InstructionList> IL);

private:
    std::ostream &os;
};

class InstructionPrinter : public InstructionVisitor {
public:
    InstructionPrinter(std::ostream &os)
        : os_{ os }
    { }

    std::ostream &os() { return os_; }
#define DECLARE_VIRTUAL_METHOD(Instr, Class)    \
    void Visit(Class##Instruction *instr) override;
INSTRUCTION_LIST_FOR_EACH(DECLARE_VIRTUAL_METHOD)
#undef DECLARE_VIRTUAL_METHOD

private:
    std::ostream &os_;
};

}
}

#endif

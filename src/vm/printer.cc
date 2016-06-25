#include "vm/printer.h"

namespace grok {
namespace vm {

void Printer::Print(std::shared_ptr<InstructionList> IL)
{
    std::string stringrep;
    for (auto &I : *IL) {
        stringrep = InstructionToString(*I);
        os << stringrep << std::endl;
    }
}

}
}

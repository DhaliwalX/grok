#include "vm/printer.h"

#include <iomanip>
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

#define PRINT_ZERO_OPERAND_INSTRUCTION(InstrClass, Instr)   \
void InstructionPrinter::Visit(InstrClass *instr)   \
{   \
    os() << #Instr << std::endl; \
}

PRINT_ZERO_OPERAND_INSTRUCTION(NoopInstruction, noop)
PRINT_ZERO_OPERAND_INSTRUCTION(PopInstruction, pop)
PRINT_ZERO_OPERAND_INSTRUCTION(PushimInstruction, pushim)
PRINT_ZERO_OPERAND_INSTRUCTION(PushThisInstruction, pushthis)
PRINT_ZERO_OPERAND_INSTRUCTION(IncInstruction, inc)
PRINT_ZERO_OPERAND_INSTRUCTION(DecInstruction, dec)
PRINT_ZERO_OPERAND_INSTRUCTION(BNotInstruction, bnot)
PRINT_ZERO_OPERAND_INSTRUCTION(SNotInstruction, snot)
PRINT_ZERO_OPERAND_INSTRUCTION(PincInstruction, pinc)
PRINT_ZERO_OPERAND_INSTRUCTION(PdecInstruction, pdec)
PRINT_ZERO_OPERAND_INSTRUCTION(LtsInstruction, lts)
PRINT_ZERO_OPERAND_INSTRUCTION(GtsInstruction, gts)
PRINT_ZERO_OPERAND_INSTRUCTION(LtesInstruction, ltes)
PRINT_ZERO_OPERAND_INSTRUCTION(GtesInstruction, gtes)
PRINT_ZERO_OPERAND_INSTRUCTION(EqsInstruction, eqs)
PRINT_ZERO_OPERAND_INSTRUCTION(NEqsInstruction, neqs)
PRINT_ZERO_OPERAND_INSTRUCTION(AddsInstruction, adds)
PRINT_ZERO_OPERAND_INSTRUCTION(SubsInstruction, subs)
PRINT_ZERO_OPERAND_INSTRUCTION(MulsInstruction, muls)
PRINT_ZERO_OPERAND_INSTRUCTION(DivsInstruction, divs)
PRINT_ZERO_OPERAND_INSTRUCTION(ShlsInstruction, shls)
PRINT_ZERO_OPERAND_INSTRUCTION(ShrsInstruction, shrs)
PRINT_ZERO_OPERAND_INSTRUCTION(RemsInstruction, rems)
PRINT_ZERO_OPERAND_INSTRUCTION(BorsInstruction, bors)
PRINT_ZERO_OPERAND_INSTRUCTION(BandsInstruction, bands)
PRINT_ZERO_OPERAND_INSTRUCTION(OrsInstruction, ors)
PRINT_ZERO_OPERAND_INSTRUCTION(AndsInstruction, ands)
PRINT_ZERO_OPERAND_INSTRUCTION(XorsInstruction, xors)
PRINT_ZERO_OPERAND_INSTRUCTION(MemCallInstruction, mem_call)
PRINT_ZERO_OPERAND_INSTRUCTION(LeaveInstruction, leave)
PRINT_ZERO_OPERAND_INSTRUCTION(MarkstInstruction, martst)
PRINT_ZERO_OPERAND_INSTRUCTION(RetInstruction, ret)

void InstructionPrinter::Visit(FetchInstruction *instr)
{
    os() << "fetch\t" << instr->name() << std::endl;
}

void InstructionPrinter::Visit(StoreInstruction *instr)
{
    os() << "store\t" << instr->name() << std::endl;
}

void InstructionPrinter::Visit(PushInstruction *instr)
{
    os() << "push\t" << std::ios::hex << instr->handle() << std::endl; 
}

#define PRINT_COUNT_INSTRUCTION(Instr, kind)    \
void InstructionPrinter::Visit(Instr *instr)   \
{   \
    os() << #kind "\t" << instr->count() << std::endl; \
}
PRINT_COUNT_INSTRUCTION(PopPropInstruction, poprop)
PRINT_COUNT_INSTRUCTION(IndexInstruction, index)
PRINT_COUNT_INSTRUCTION(ResInstruction, res)
PRINT_COUNT_INSTRUCTION(CopyAInstruction, copya)
PRINT_COUNT_INSTRUCTION(LoopzInstruction, loopz)
PRINT_COUNT_INSTRUCTION(JmpInstruction, jmp)
PRINT_COUNT_INSTRUCTION(JmpzInstruction, jmpz)
PRINT_COUNT_INSTRUCTION(JmpnzInstruction, jmpnz)
PRINT_COUNT_INSTRUCTION(CallInstruction, call)
#undef PRINT_COUNT_INSTRUCTION

#define PRINT_NAME_INSTRUCTION(Instr, kind)    \
void InstructionPrinter::Visit(Instr *instr)   \
{   \
    os() << #kind "\t" << instr->name() << std::endl; \
}

PRINT_NAME_INSTRUCTION(ReplPropInstruction, replprop)
PRINT_NAME_INSTRUCTION(MapsInstruction, maps)
PRINT_NAME_INSTRUCTION(NewsInstruction, news)


#undef PRINT_ZERO_OPERAND_INSTRUCTION


}
}

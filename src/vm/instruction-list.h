#ifndef INSTRUCTION_LIST_H_
#define INSTRUCTION_LIST_H_

#include "vm/instruction.h"
#include <memory>
#include <vector>

namespace grok {
namespace vm {

/// Instruction ::= data structure to hold instruction
using Instruction = RawInstruction<int>;

/// InstructionList ::= list that will store the instructions in a
/// contigous memory, for now it stores the instructions in a std::vector
/// I used vector because of its automatic expansion on requirements
using InstructionList = std::vector<std::shared_ptr<Instruction>>;

/// InstructionBlock ::= class used for storing blocks of instructions
/// in terms of InstructionList
class InstructionBlock {
public:
    InstructionBlock();
    InstructionBlock(const InstructionBlock&) = delete;
    InstructionBlock &operator=(const InstructionBlock&) = delete;
    ~InstructionBlock() = default;

    /// Append ::= append instruction
    void Append(std::shared_ptr<Instruction> instr);

    /// Append ::= append another block
    void AppendBlock(std::shared_ptr<InstructionBlock> block);

    /// AppendBlockForJump ::= appends the block only if the last instruction
    /// in the current block is a jump instruction, the length of the jump
    /// instruction will be calculated from the block to be appended
    void AppendBlockForJump(std::shared_ptr<InstructionBlock> block);

    /// ReleaseBlock ::= releases block so created,
    /// NOTE: don't use the object after that, may result in SEG FAULT
    std::unique_ptr<InstructionList> ReleaseBlock();

    /// Length ::= returns the length of the block
    std::size_t Length() const;

    /// UpdateStackedJump ::= updated the length of jump to size
    void UpdateStackedJump(std::size_t size);
private:
    std::unique_ptr<InstructionList> list_;
};

} // vm
} // grok

#endif

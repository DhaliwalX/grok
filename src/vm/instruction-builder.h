#ifndef INSTRUCTION_BUILDER_H_
#define INSTRUCTION_BUILDER_H_

#include "vm/instruction.h"
#include "vm/instruction-list.h"
#include "common/util.h"

#include <list> // needed for stack operations

namespace grok {
namespace vm {

/// BlockStack ::= class representing stacks for storing the blocks
using BlockStack = std::list<std::shared_ptr<InstructionBlock>>;

/// InstructionBuilder ::= helper class for creating instructions
/// and adding the instructions to the instruction list so that they
/// can be easily interpreted by vm
class InstructionBuilder {
    InstructionBuilder() = default;
public:

    /// Create ::= creates a new instruction on heap and return it
    template <Instructions instr>
    static std::shared_ptr<Instruction> Create()
    {
        auto inst = std::make_shared<Instruction>();
        inst->kind_ = instr;
        return inst;
    }

    /// CreateBlock ::= creates a new block and from now on all the 
    ///     instructions added will be appended to this block
    void CreateBlock();

    /// AddInstruction ::= adds the instruction to the current block
    void AddInstruction(std::shared_ptr<Instruction> instr);

    /// EndBlock ::= this Instruction block has ended and it will be appended
    /// to the one that is currently in the stack
    void EndBlock();

    /// EndBlockForJump ::= previous block held in stack was delayed due to
    /// a jump instruction
    void EndBlockForJump();

    /// UpdateStackedJump ::= updates the value of jmp length which is in stack
    void UpdateStackedJump();

    /// Finalize ::= finalizes the InstructionList
    void Finalize();

    /// ReleaseInstructionList ::= releases the finalized instruction list
    std::unique_ptr<InstructionList> ReleaseInstructionList();

    /// CurrentLength() ::= returns the length of the working block
    auto CurrentLength() const { return working_block_->Length(); }

    static std::unique_ptr<InstructionBuilder> CreateBuilder()
    {
        std::unique_ptr<InstructionBuilder> builder{ new InstructionBuilder() };
        builder->good_state_ = true;

        return std::move(builder);
    }

private:

    bool good_state_; // 0 for not good, 1 for good
    BlockStack blockstack_;
    std::shared_ptr<InstructionBlock> working_block_;
};


}
}

#endif // instruction-builder.h

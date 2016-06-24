#include "vm/instruction-builder.cc"

namespace grok {
namespace vm {

void InstructionBuilder::CreateBlock()
{
    auto newblock = std::make_shared<InstructionBlock>();

    // push the working_block_ to stack
    if (working_block_)
        blockstack_.push_back(working_block_);

    good_state_ = false;
    // switch to new block
    working_block_ = newblock;
}

void InstructionBuilder::AddInstruction(std::unique_ptr<Instruction> instr)
{
    if (!working_block_)
        throw std::runtime_error("fatal: working block was nullptr, "
            "was expected to be initialized");

    // append the instruction
    working_block_->Append(std::move(instr));
}

void InstructionBuilder::EndBlock()
{
    if (!working_block_)
        throw std::runtime_error("fatal: working block was nullptr, "
            "was expected to be initialized");

    if (blockstack_.empty()) {
        return;
    } else {
        auto block = blockstack_.back();
        blockstack_.pop_back();

        block->AppendBlock(working_block_);
        working_block_ = block;
    }
}

void InstructionBuilder::EndBlockForJump()
{
    if (!working_block_)
        throw std::runtime_error("fatal: working block was nullptr, "
            "was expected to be initialized");

    if (blockstack_.empty()) {
        good_state_ = true;
        return;
    } else {
        auto block = blockstack_.back();
        blockstack_.pop_back();

        block->AppendBlockForJump(working_block_);
        working_block_.reset();
        working_block_ = block;
    }

    if (blockstack_.empty()) {
        good_state_ = true;
    }
}

void InstructionBuilder::Finalize()
{
    if (blockstack_.empty())
        return; // already finalized

    while (!blockstack_.empty()) {
        auto ablock = blockstack_.back();
        blockstack_.pop_back();

        // merge two blocks
        ablock->AppendBlock(working_block_);
        working_block_.reset();
        working_block_ = ablock;
    }
    good_state_ = true;
}

std::unique_ptr<InstructionList> InstructionBuilder::ReleaseInstructionList()
{
    if (!good_state_)
        throw std::runtime_error("forgot to call Finalize()?");
    auto ret = working_block_->ReleaseBlock();
    return std::move(ret);
}

void InstructionBuilder::UpdateStackedJump()
{
    auto last_block = blockstack_.back();
    auto size = working_block_.size();

    last_block->UpdateStackedJump(size);
}

} // vm
} // grok

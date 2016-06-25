#include "vm/instruction-list.h"

namespace grok {
namespace vm {

InstructionBlock::InstructionBlock()
    : list_ { std::make_unique<InstructionList>() }
{ }

void InstructionBlock::Append(std::shared_ptr<Instruction> instr)
{
    if (!list_)
        throw std::runtime_error("fatal: list_ was nullptr");
    list_->push_back(instr);
}

void InstructionBlock::AppendBlock(std::shared_ptr<InstructionBlock> block)
{
    // get the list of the second block
    // Note `block` will be invalidated now
    auto list = block->ReleaseBlock();
    list_->insert(list_->end(), list->begin(), list->end());
}

void InstructionBlock::AppendBlockForJump(
                    std::shared_ptr<InstructionBlock> block)
{
    auto instr = list_->back().get();

    // we expected a kind of jump instruction only
    if (!instr->IsKindOfJump())
        throw std::runtime_error("fatal: AppendBlockForJump was called with"
            "no prior jmp instruction");

    // set the length of the jump
    auto jmp_sz = block->Length();
    instr->SetJumpLength(jmp_sz);

    AppendBlock((block));
}

std::unique_ptr<InstructionList> InstructionBlock::ReleaseBlock()
{
    return std::move(list_);
}

std::size_t InstructionBlock::Length() const
{
    return list_->size();
}

void InstructionBlock::UpdateStackedJump(std::size_t size)
{
    auto instr = list_->back().get();
    instr->SetJumpLength(size);
}

} // vm
} // grok


#include "vm/codegen.h"

namespace grok {
namespace vm {

CodeGenerator::CodeGenerator()
    : IR{}, Builder{}
{
    Builder = InstructionBuilder::CreateBuilder();
}

void CodeGenerator::Generate(grok::parser::Expression *AST)
{
    Builder->CreateBlock();
    if (Builder->InsideFunction()) {
        auto nop = InstructionBuilder::Create<Instructions::noop>();
        Builder->AddInstruction(std::move(nop));
    }
    AST->emit(Builder);
    if (Builder->InsideFunction()) {
        auto ret = InstructionBuilder::Create<Instructions::ret>();
        Builder->AddInstruction(std::move(ret));
    }
    Builder->EndBlock();
    Builder->Finalize();
    IR = Builder->ReleaseInstructionList();
}

std::shared_ptr<InstructionList> CodeGenerator::GetIR()
{
    return IR;
}

}
}

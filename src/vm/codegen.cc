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
    AST->emit(Builder);
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

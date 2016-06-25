#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "parser/expression.h"
#include "vm/instruction.h"
#include "vm/instruction-list.h"
#include "vm/instruction-builder.h"

#include <iostream>

namespace grok {
namespace vm {

/// CodeGenerator ::= takes AST and generates code from it
class CodeGenerator {
public:
    CodeGenerator();

    /// Generate ::= generates code from AST
    void Generate(grok::parser::Expression *AST);

    /// GetIR ::= returns IR
    std::shared_ptr<InstructionList> GetIR();
private:
    std::shared_ptr<InstructionList> IR;
    std::shared_ptr<InstructionBuilder> Builder;
};

} // vm
} // grok

#endif // codegen.h

#include "codegen.h"

namespace Code {

unsigned long CodeGen::addr_ = 0;        // define the static variable

bool CodeGen::GenerateForMemberIdent(
                  BytecodeProgram<Register, Bytecode>* program,
                             std::shared_ptr<AstNode> node) {
  auto ident_child = node->links_[0];
  auto var_name = ident_child->variable_.GetName();
  program->text_.push_back(B::opushkey(var_name));
  return true;
}

bool CodeGen::GenerateForMemberChild(
                BytecodeProgram<Register, Bytecode>* program,
                std::shared_ptr<AstNode> node) { 
  // the expression_type_ member is always primary
  switch (node->relation2_) {
  case DOT:
    // the expression is of type x.y
    program->text_.push_back(B::opushobjprop(node->variable_.GetName()));
    break;

  case LSQB:
    // expression is of type x[3 + 4]
    __gen_code(program, node);
    program->text_.push_back(B::opusharrobj(0));
    break;

  default:
    // not implemented yet
    printf("not implemented yet\n");
    return false;
  }
  return true;
}

bool CodeGen::GenerateForMemberChildren(
                  BytecodeProgram<Register, Bytecode>* program,
                             std::shared_ptr<AstNode> node) {
  if (node->links_.size() < 2) {
    printf("FatalError: \nIn GenerateForMemberChildren()> ");
    printf("The size of children was %d while generating code");
    printf(" for member expression\n", node->links_.size());
    return false;
  }

  // each member expression has second children actual member
  auto it = node->links_.begin() + 1;
  for (; it != node->links_.end(); ++it)
    if (!GenerateForMemberChild(program, *it))
      return false;
  return true;
}

bool 
CodeGen::GenerateMember(BytecodeProgram<Register, Bytecode>* program,
                             std::shared_ptr<AstNode> node) {
  size_t child_count = node->links_.size();
  if (!GenerateForMemberIdent(program, node)) {
    return false;
  }
  if (!GenerateForMemberChildren(program, node)) {
    return false;
  }
  program->text_.push_back(B::opopts());
  return true;
}

bool 
CodeGen::GenerateMemberW(BytecodeProgram<Register, Bytecode>* program,
                              std::shared_ptr<AstNode> node) {
  size_t child_count = node->links_.size();
  if (!GenerateForMemberIdent(program, node)) {
    return false;
  }
  if (!GenerateForMemberChildren(program, node)) {
    return false;
  }
  // for writing to the object we don't have to move the object to the stack.
  // `program->text_.push_back(B::opopts());`
  return true;
}


} // namespace Code;

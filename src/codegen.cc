#include "codegen.h"

namespace Code {

unsigned long CodeGen::addr_ = 0; // define the static variable

bool CodeGen::GenerateForMemberIdent(
    BytecodeProgram<Register, Bytecode> *program,
    std::shared_ptr<AstNode> node) {
  auto ident_child = node->links_[0];
  auto var_name = ident_child->variable_.GetName();
  program->text_.push_back(B::opushkey(var_name));
  return true;
}

bool CodeGen::GenerateUnary(BytecodeProgram<Register, Bytecode> *program,
                            std::shared_ptr<AstNode> node) {
  switch (node->relation1_) {
  case PLUS:
    return __gen_code(program, node->links_[0]);

  case MINUS:
    if (__gen_code(program, node->links_[0]))
      program->text_.push_back(B::neg());
    else
      return false;
    return true;

  case INC:
  case DEC:
    if (node->relation2_ == PLUS)
      return GeneratePostOperator(program, node);
    else
      return GeneratePreOperator(program, node);

  default:
    return false;
  }
  return false;
}

bool CodeGen::GeneratePostOperator(BytecodeProgram<Register, Bytecode> *program,
                                   std::shared_ptr<AstNode> node) {
  switch (node->relation1_) {
  case INC:
    if (__gen_code(program, node->links_[0]))
      program->text_.push_back(B::pinc());
    else
      return false;
    break;

  case DEC:
    if (__gen_code(program, node->links_[0]))
      program->text_.push_back(B::pdec());
    else
      return false;
    break;
  }
  return true;
}

bool CodeGen::GeneratePreOperator(BytecodeProgram<Register, Bytecode> *program,
                                  std::shared_ptr<AstNode> node) {
  switch (node->relation1_) {
  case INC:
    if (__gen_code(program, node->links_[0]))
      program->text_.push_back(B::inc());
    else
      return false;

  case DEC:
    if (__gen_code(program, node->links_[0]))
      program->text_.push_back(B::dec());
    else
      return false;
  }
  return true;
}

bool CodeGen::GenerateForMemberChild(
    BytecodeProgram<Register, Bytecode> *program,
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

  case LPAR:
    return GenerateFunctionCall(program, node);
  }
  return true;
}

bool CodeGen::GenerateFunctionCall(BytecodeProgram<Register, Bytecode> *program,
                                   std::shared_ptr<AstNode> node) {
  auto argument_number = node->links_.size();
  if (argument_number > 0)
    for (auto it = node->links_.begin(); it != node->links_.end(); ++it) {
      if (!__gen_code(program, *it))
        return false;
    }
  Register temp;
  temp.value_ = argument_number;
  program->text_.push_back(B::call(temp));
  return true;
}

bool CodeGen::GenerateForMemberChildren(
    BytecodeProgram<Register, Bytecode> *program,
    std::shared_ptr<AstNode> node) {
  if (node->links_.size() < 2) {
    printf("FatalError: \nIn GenerateForMemberChildren()> ");
    printf("The size of children was %d while generating code",
           node->links_.size());
    printf(" for member expression\n");
    return false;
  }

  // each member expression has second children actual member
  auto it = node->links_.begin() + 1;
  for (; it != node->links_.end(); ++it)
    if (!GenerateForMemberChild(program, *it))
      return false;
  return true;
}

bool CodeGen::GenerateMember(BytecodeProgram<Register, Bytecode> *program,
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

bool CodeGen::GenerateMemberW(BytecodeProgram<Register, Bytecode> *program,
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

int CodeGen::GenerateFunction(BytecodeProgram<Register, Bytecode> *program,
                              std::shared_ptr<AstNode> node, Machine *machine) {
  auto func = std::dynamic_pointer_cast<JSFunction, JSBasicObject>(node->obj_);
  if (func->IsNative())
    return false;

  auto func_body = func->GetJSHandle();
  BytecodeProgram<Register, Bytecode> *function_body =
      new BytecodeProgram<Register, Bytecode>();

  if (!Code::__GenerateCode(this, function_body, func_body, machine)) {
    printf("Code generation failed for function %s\n", func->GetName().c_str());
    return 0;
  }

  func->function_body_ = function_body;
  std::shared_ptr<AstNode> function_node = std::make_shared<AstNode>();
  function_node->obj_ = func;

  Heap::heap.PushVariable({func->GetName(), function_node});

  function_body->print(std::cout);
  return 1;
}

} // namespace Code;

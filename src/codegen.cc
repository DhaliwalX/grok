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

  case LPAR:
    // not implemented yet
    return GenerateFunctionCall(program, node);
  }
  return true;
}

bool CodeGen::GenerateFunctionCall(
                BytecodeProgram<Register, Bytecode>* program,
                    std::shared_ptr<AstNode> node) {
  auto func_name = node->links_[0]->variable_.GetName();
  auto argument_number = node->links_[1]->links_.size();
  for (auto it = node->links_[1]->links_.begin();
      it != node->links_[1]->links_.end(); ++it) {
    if (!__gen_code(program, *it))
      return false;
  }
  Register temp;
  temp.object_ = nullptr;
  temp.str_ = func_name;
  temp.value_ = argument_number;
  program->text_.push_back(B::call(temp));
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

int 
CodeGen::GenerateFunction(BytecodeProgram<Register, Bytecode>* program,
                          std::shared_ptr<AstNode> node) {
  auto func = std::dynamic_pointer_cast<JSFunction, JSBasicObject>(node->obj_);
  if (func->IsNative())
    return false;
  
  auto func_body = func->GetJSHandle();
  BytecodeProgram<Register, Bytecode> *function_body
    = new BytecodeProgram<Register, Bytecode>();
  
  if (!Code::GenerateCode(this, function_body, func_body, machine_))
    return 0;

  func->function_body_ = function_body;
  std::shared_ptr<AstNode> function_node = std::make_shared<AstNode>();
  function_node->obj_ = func;

  Heap::heap.PushVariable({ func->GetName(), function_node });
  return 1;
}


} // namespace Code;

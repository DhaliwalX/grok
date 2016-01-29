#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "bytecode.h"
#include "astnode.h"
#include "program.h"
#include "symtable.h"
#include "jsobject.h"
#include "jsarray.h"
#include "object.h"
#include "jsfunction.h"
#include "vm.h"

#include <iostream>
#include <string>
#include <vector>  // stack
#include <memory>

#define JS JSBasicObject::ObjectType
#define REGISTER_FOR_CONDITION 7
namespace Code {

class CodeGen;
static bool GenerateCode(CodeGen *codegen,
                         BytecodeProgram<Register, Bytecode> *p,
                         std::shared_ptr<AstNode> node, Machine *machine);

#define CASES()                                   \
    case AstNode::ExpressionType::_or:            \
    case AstNode::ExpressionType::_and:           \
    case AstNode::ExpressionType::_bitor:         \
    case AstNode::ExpressionType::_bitxor:        \
    case AstNode::ExpressionType::_bitand:        \
    case AstNode::ExpressionType::_equality:      \
    case AstNode::ExpressionType::_relational:    \
    case AstNode::ExpressionType::_shift:         \
    case AstNode::ExpressionType::_term:          \
    case AstNode::ExpressionType::_factor

// CodeGen class : generates code for various expressions
class CodeGen {
public:

  bool __gen_code(BytecodeProgram<Register, Bytecode> *program,
                  std::shared_ptr<AstNode> node) {
    current_ = node;
    bool status = true;

    switch (current_->expression_type_) {
    case AstNode::ExpressionType::_empty:
      // no need to generate code for empty
      break;

    case AstNode::ExpressionType::_primary:
      addr_stack_.push_back(PrimaryExpression(program, current_));
      break;

    CASES():
      __gen_code(program, node->links_[0]);
      __gen_code(program, node->links_[1]);
      GenInstruction(program, node);
      break;

    case AstNode::ExpressionType::_assignment:
      status = AssignmentExpression(program, current_);
      break;

    }
    return true;
  err:
    stack_.clear();
    return status;
  }

  // generate code for assignment expression
  bool AssignmentExpression(BytecodeProgram<Register, Bytecode> *program,
                            std::shared_ptr<AstNode> node) {
    if (node->links_[0]->relation1_ != IDENT) {
      printf("Can't assign to a rvalue\n");
      return false;
    }
    __gen_code(program, node->links_[1]);
    int addr = AddSymbol(program, node->links_[0]);
    program->text_.push_back(B::popkey(Register(
            node->links_[0]->variable_.GetName())));
    program->text_.push_back(B::pushkey(Register(
      node->links_[0]->variable_.GetName())));
    return true;
  }

#define CASE(a, b)                                     \
case a:                                                \
    program->text_.push_back(B::b());                  \
    break;

  // generate instruction for binary operations
  void GenInstruction(BytecodeProgram<Register, Bytecode> *program,
                      std::shared_ptr<AstNode> node) {
    switch (node->relation1_) {
      CASE(PLUS, add);
      CASE(MINUS, sub);
      CASE(MUL, mul);
      CASE(DIV, div);
      CASE(MOD, mod);
      CASE(SHR, shr);
      CASE(SHL, shl);
      CASE(LT, lt);
      CASE(GT, gt);
      CASE(EQUAL, equal);
      CASE(NOTEQ, _not_eq);

    default:
      return;
    }
    return;
  }
#undef CASE

  int PrimaryExpression(BytecodeProgram<Register, Bytecode> *program,
                        std::shared_ptr<AstNode> node) {
    int addr = 0;

    switch (node->relation1_) {
    case IDENT:
      AddSymbol(program, node);
      program->text_.push_back(B::pushkey(Register(node->variable_.GetName())));
      break;

    case DIGIT:
      program->text_.push_back(B::pushi(Register(node->obj_)));
      break;

    case STRING:
      program->text_.push_back(B::pushi(Register(node->obj_)));
      break;

    case LSQB:
      GenerateArray(program, node);
      break;

    case LBRACE:
      GenerateObject(program, node);
      break;

    //case LPAR:
    //  GenerateFunction(program, node);
    //  break;

    default:
      return -1;
    }

    return addr;
  }

  int GenerateObject(BytecodeProgram<Register, Bytecode> *program,
                     std::shared_ptr<AstNode> node) {
    program->text_.push_back(B::opushobj());
    for (auto it = node->links_.begin(); it != node->links_.end(); it++) {
      if (!__gen_code(program, (*it)->links_[0]))
        return 0;
      program->text_.push_back(B::opoptobj(
        Register((*it)->variable_.GetName())));
    }
    program->text_.push_back(B::opopts());
    return 1;
  }

  int GenerateArray(BytecodeProgram<Register, Bytecode> *program,
                    std::shared_ptr<AstNode> node) {
    program->text_.push_back(B::opusharr());
    for (auto it = node->links_.begin(); it != node->links_.end(); it++) {
      if (!__gen_code(program, *it))
        return 0;
      program->text_.push_back(B::opoptarr());
    }
    program->text_.push_back(B::opopts());
    return 1;
  }

  int AddSymbol(BytecodeProgram<Register, Bytecode> *program,
                std::shared_ptr<AstNode> node) {
    auto v = js::heap.FindVariable(node->variable_.GetName());
    
    // if symbol already exists return the address of that variable
    if (v.get()
        && v->expression_type_ == AstNode::ExpressionType::_undefined)
      return 1;

    auto temp = std::make_shared<AstNode>();
    temp->obj_ = std::make_shared<JSBasicObject>();
    temp->expression_type_ = AstNode::ExpressionType::_primary;
    // else create an undefined object in current scope
    js::heap.PushVariable({ node->variable_.GetName(),
                             temp });    
    return 1;
  }

  void UpdateStack(std::shared_ptr<AstNode> node) {
    if (node->expression_type_ == AstNode::ExpressionType::_assignment) {
      stack_.push_back(node);
      stack_.push_back(node->links_.back());
      node->links_.pop_back();
      return;
    }

    stack_.push_back(node);
    for (auto &child : node->links_)
      stack_.push_back(child);
    node->links_.clear();
  }

  // generate the code for the program
  // for this is a simple testing
  bool GenerateCode(BytecodeProgram<Register, Bytecode> *program,
                    std::shared_ptr<AstNode> node, Machine *machine) {
    if (!program && !node.get())
      return false;

    machine_ = machine;

    stack_.clear();
    addr_stack_.clear();
    return __gen_code(program, node);
  }


  static unsigned long addr_;
private:
  // needed for postorder evaluation
  std::vector<std::shared_ptr<AstNode>> stack_;

  std::shared_ptr<AstNode> current_;    // current node being evaluated
  std::vector<int> addr_stack_;
  Machine *machine_;
};

unsigned long CodeGen::addr_ = 0;        // define the static variable

static bool GenerateIf(CodeGen *codegen, 
                       BytecodeProgram<Register, Bytecode> *p,
                       std::shared_ptr<AstNode> node,
                       Machine *machine) {
  if (!codegen->GenerateCode(p, node->links_[0], machine))
    return false;
  
  int i = p->text_.size();

  // last register is always used for conditional expression
  p->text_.push_back(B::popa());
  p->text_.push_back(B::jmpz());
  p->text_.push_back(B::jmp(1));

  if (!GenerateCode(codegen, p, node->links_[1], machine))
    return false;

  int size = p->text_.size() - i; 
  p->text_.insert(p->text_.begin() + i + 3, B::jmp(size - 3));
  
  size = p->text_.size();
  if ((node->links_.size() == 3)
      && !GenerateCode(codegen, p, node->links_[2], machine)) {
    int s = p->text_.size() - size;
    p->text_.insert(p->text_.begin() + size, B::jmp(s));
    return true;
  }

  return true;
}

static bool GenerateWhile(CodeGen *codegen,
                          BytecodeProgram<Register, Bytecode> *p,
                          std::shared_ptr<AstNode> node,
                          Machine *machine) {
  int start = p->text_.size();
  if (!codegen->GenerateCode(p, node->links_[0], machine))
    return false;

  int i = p->text_.size() - start;
  // last register is always used for conditional expression
  p->text_.push_back(B::popa());
  p->text_.push_back(B::jmpz());
  p->text_.push_back(B::jmp(1));

  if (!GenerateCode(codegen, p, node->links_[1], machine))
    return false;
  p->text_.push_back(B::empty());

  int jmp_back = p->text_.size() - start + 2;
  p->text_.push_back(B::jmp(-jmp_back));

  int size = p->text_.size() - i - start;
  p->text_.insert(p->text_.begin() + (start + i + 3), B::jmp(size - 3));
  return true;
}

static bool GenerateFor(CodeGen *codegen,
                        BytecodeProgram<Register, Bytecode> *p,
                        std::shared_ptr<AstNode> node,
                        Machine *machine) {
  if (!codegen->GenerateCode(p, node->links_[0], machine))
    return false;
  int start = p->text_.size();

  if (!codegen->GenerateCode(p, node->links_[1], machine))
    return false;

  int i = p->text_.size() - start;

  // last register is always used for conditional expression
  p->text_.push_back(B::popa());
  p->text_.push_back(B::jmpz());
  p->text_.push_back(B::jmp(1));

  if (!GenerateCode(codegen, p, node->links_[3], machine))
    return false;

  if (!codegen->GenerateCode(p, node->links_[2], machine))
    return false;

  p->text_.push_back(B::empty());

  int jmp_back = p->text_.size() - start + 2;
  p->text_.push_back(B::jmp(-jmp_back));

  int size = p->text_.size() - start - i;
  p->text_.insert(p->text_.begin() + (start + i + 3), B::jmp(size - 3));
  return true;
}

static bool GenerateBlock(CodeGen *codegen,
                          BytecodeProgram<Register, Bytecode> *p,
                          std::shared_ptr<AstNode> node,
                          Machine *machine) {
  if (!node->links_.size())
    return true;

  for (auto &child : node->links_) {
    if (!GenerateCode(codegen, p, child, machine))
      return false;
    p->text_.push_back(B::empty());
  }
}

static bool __GenerateCode(CodeGen *codegen,
                         BytecodeProgram<Register, Bytecode> *p,
                         std::shared_ptr<AstNode> node, 
                           Machine *machine) {
  switch (node->expression_type_) {
  case AstNode::ExpressionType::_for:
    return GenerateFor(codegen, p, node, machine);

  case AstNode::ExpressionType::_while:
    return GenerateWhile(codegen, p, node, machine);

  case AstNode::ExpressionType::_if:
    return GenerateIf(codegen, p, node, machine);

  case AstNode::ExpressionType::_compound:
    return GenerateBlock(codegen, p, node, machine);

  case AstNode::ExpressionType::_function:
    ;

  default:
    return codegen->GenerateCode(p, node, machine);
  }
}

static bool GenerateCode(CodeGen *codegen,
                           BytecodeProgram<Register, Bytecode> *p,
                           std::shared_ptr<AstNode> node,
                         Machine *machine) {
  if (!node.get() && !p && !codegen) {
    printf("tree or program or codegen was null\n");
    return false;
  }

  if (!p)
    p = new BytecodeProgram<Register, Bytecode>();

  if (node->links_.size())
    for (auto each : node->links_) {
      __GenerateCode(codegen, p, each, machine);
    }
}

} // namespace Code

#undef CASES
#endif
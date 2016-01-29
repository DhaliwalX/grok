// vm.c

#include "vm.h"

#include <cstdio>
#include <exception>

#define NextInstruction()                                     \
  (text_.at(instruction_register_.value_++))

#define Continue()                                            \
  { bytecode = NextInstruction();                             \
  continue; }

#define StopMachine()                                         \
  bytecode.instruction_ = static_cast<ui>(Instruction::HLT);  \
  continue;

// calculate effective address
// if second field is true then the operand is immediate
std::pair<unsigned long long, bool> 
  Machine::effective_address(Bytecode &code) {
  switch ((code.instruction_ >> 9) & 0x3) {
  case _Imm:         // Imm
    return{ 0, true };

  case _Adr:           // adr
    return{ code.value_.value_, false };

  case _Reg:           // reg
    return{ registers_[code.value_.value_ & 0xf].value_, false };

  case _Idx:           // idx
    return{ code.value_.value_ + idx_.value_, false };
  }
}

#define J JSBasicObject::ObjectType

J GetType(Register &reg) {
  switch (reg.type_) {
  case DataType::Number:
    return JSBasicObject::ObjectType::_number;

  case DataType::String:
    return JSBasicObject::ObjectType::_string;

  case DataType::Object:
  {
    if (reg.object_.get()) {
      return reg.object_->GetType();
    }
  }
  }
  return J::_undefined;
}

void Machine::report_error(const std::string &str) {
  std::fprintf(stdout, "%s", str.c_str());
}

void Machine::script_error(const std::string &str) {

}

void Machine::print_stack_trace() {

}

void Machine::decode_registers(int reg,
                               int &a,
                               int &b,
                               int &c) {
  a = reg & 0x7;
  reg >>= 3;
  b = reg & 0x7;
  reg >>= 3;
  c = reg & 0x7;
}

bool Machine::check_if_valid(int a, int b, int c) {
  return !(a > 0x7) || !(b > 0x7) || !(c > 0x7);
}

Register Machine::calculate_operand(int r) {
  Register res;
  try {
    res = data_[registers_[r].value_];
  }
  catch (std::exception e) {
    return Register();
  }
  return res;
}

J decode_type(Register r2, Register r3) {
  auto t1 = r2.object_->GetType();
  auto t2 = r3.object_->GetType();
  if (t1 == J::_number
      && t2 == J::_number)
    return t1;

  if ((t1 == J::_string
       && t2 == J::_number)
      || (t1 == J::_number
          && t2 == J::_string)
      || (t1 == J::_string
          && t2 == J::_string))
    return J::_string;

  if (t1 == J::_object
      || t2 == J::_object)
    return J::_object;

  if (t1 == J::_array
      || t2 == J::_array)
    return J::_array;

  return J::_undefined;
}

void Machine::set_object(Register &res) {
  data_[registers_[(reg_ >> 8) & 0xf].value_] = res;
}

bool Machine::binary_add() {
  Register a, b, res;
  a = stack_.back();
  stack_.pop_back();

  b = stack_.back();
  stack_.pop_back();

  auto type = ::decode_type(a, b);
  switch (type) {
  case J::_number:
    res.object_ = std::dynamic_pointer_cast<JSBasicObject,
      JSNumber>(std::make_shared<JSNumber>(
      a.object_->GetNumber() + b.object_->GetNumber()));
    res.type_ = DataType::Object;
    break;

  case J::_string:
    res.object_ = std::dynamic_pointer_cast<JSBasicObject,
      JSString>(std::make_shared<JSString>(
        a.object_->ToString() + b.object_->ToString()));
    res.type_ = DataType::Object;
    break;

  default:
    res = Register();
  }

  stack_.push_back(res);
  return true;
}

#define BINARYOP(name, op)                                    \
bool Machine::name() {                                        \
  Register a, b, res;                                         \
  b = stack_.back();                                          \
  stack_.pop_back();                                          \
  a = stack_.back();                                          \
  stack_.pop_back();                                          \
  auto type = ::decode_type(a, b);                            \
  switch (type) {                                             \
  case J::_number:                                            \
    res.object_ = std::dynamic_pointer_cast<JSBasicObject,    \
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.object_->GetNumber() op b.object_->GetNumber()));   \
    res.type_ = DataType::Object;                             \
    break;                                                    \
                                                              \
                                                              \
  default:                                                    \
    res = Register();                                         \
  }                                                           \
                                                              \
  stack_.push_back(res);                                      \
  return true;                                                \
}

BINARYOP(binary_sub, -)
BINARYOP(binary_mul, *)
BINARYOP(binary_div, /)
BINARYOP(binary_mod, / )
BINARYOP(shift_left, <<)
BINARYOP(shift_right, >>)
//BINARYOP(binary_and, &&)
//BINARYOP(binary_or, ||)
//BINARYOP(bit_and, &)
//BINARYOP(bit_or, |)

#undef BINARYOP

#define COMPOP(name, op)                                      \
bool Machine::name() {                                        \
  Register a, b, res;                                         \
  a = stack_.back();                                          \
  stack_.pop_back();                                          \
                                                              \
  b = stack_.back();                                          \
  stack_.pop_back();                                          \
                                                              \
  auto type = ::decode_type(a, b);                            \
  switch (type) {                                             \
  case J::_number:                                            \
    res.object_ = std::dynamic_pointer_cast<JSBasicObject,    \
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.object_->GetNumber() op b.object_->GetNumber()));   \
    res.type_ = DataType::Object;                             \
    break;                                                    \
                                                              \
  case J::_string:                                            \
    res.object_ = std::dynamic_pointer_cast<JSBasicObject,    \
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.object_->ToString() op b.object_->ToString()));     \
    res.type_ = DataType::Object;                             \
    break;                                                    \
                                                              \
  default:                                                    \
    res = Register(0);                                        \
  }                                                           \
                                                              \
  stack_.push_back(res);                                      \
  return true;                                                \
} 

COMPOP(do_equal, ==)
COMPOP(do_not_equal, !=)
COMPOP(do_less_than, <)
COMPOP(do_greater_than, >)

#undef COMPOP

bool Machine::is_zero(Register &reg) {
  auto type = GetType(reg);
  switch (type) {
  case J::_number:
    return !reg.object_->GetNumber();

  case J::_string:
    return !reg.object_->ToString().size();

  default:
    return false;
  }
}

bool Machine::handle_call(Bytecode &bytecode) {

  return true;
}


void Machine::execute() {
  // execute the program stored in current program pointer

  // reset the instruction_register_
  instruction_register_.reset();

  instruction_register_ = program_start_;
  Bytecode bytecode = NextInstruction();

  auto size_text = text_.size();
  bool status = true;
  std::pair<unsigned long long, bool> addr;

  while (status && (instruction_register_.Value() != size_text)
         && (static_cast<Instruction>(bytecode.instruction_)
             != Instruction::HLT)) {
    instruction_executed_++;

    switch (static_cast<Instruction>(bytecode.instruction_)) {
    case Instruction::NONE:
      bytecode = NextInstruction();
      continue;

    case Instruction::LDAI:
      accumulator_ = bytecode.value_;
      bytecode = NextInstruction();
      continue;

    case Instruction::LDADR:
      accumulator_ = data_.at(bytecode.value_.value_);
      Continue();

    case Instruction::LDAKEY:
    {
      auto obj_ = js::heap.FindVariable(bytecode.value_.str_);
      if (!obj_.get()
          || obj_->expression_type_ == AstNode::ExpressionType::_undefined) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      accumulator_ = Register(obj_->obj_);
      Continue();
    }

    case Instruction::STRA:
    {
      data_[bytecode.value_.value_] = accumulator_;
      Continue();
    }

    case Instruction::PUSHI:
      stack_.push_back(bytecode.value_);
      Continue();

    case Instruction::PUSHADR:
      stack_.push_back(data_[bytecode.value_.value_]);
      Continue();

    case Instruction::PUSHREG:
      stack_.push_back(accumulator_);
      Continue();

    case Instruction::PUSHKEY:
    {
      auto obj_ = js::heap.FindVariable(bytecode.value_.str_);
      if (!obj_.get()
          || obj_->expression_type_ == AstNode::ExpressionType::_undefined) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      stack_.push_back(Register(obj_->obj_));
      Continue();
    }

    case Instruction::OPUSHOBJ:
    {
      o_stack_.push_back(std::dynamic_pointer_cast<JSBasicObject,
                         JSObject>(std::make_shared<JSObject>()));
      Continue();
    }

    case Instruction::OPUSHARR:
    {
      o_stack_.push_back(std::dynamic_pointer_cast<JSBasicObject,
                         JSArray>(std::make_shared<JSArray>()));
      Continue();
    }

    case Instruction::LDAARR:
      accumulator_ = std::dynamic_pointer_cast<JSBasicObject,
        JSArray>(std::make_shared<JSArray>());
      Continue();

    case Instruction::LDAOBJ:
      accumulator_ = std::dynamic_pointer_cast<JSBasicObject,
        JSObject>(std::make_shared<JSObject>());
      Continue();

    case Instruction::LDANUM:
      accumulator_ = std::dynamic_pointer_cast<JSBasicObject,
        JSNumber>(std::make_shared<JSNumber>());
      Continue();

    case Instruction::LDASTR:
      accumulator_ = std::dynamic_pointer_cast<JSBasicObject,
        JSString>(std::make_shared<JSString>());
      Continue();

    case Instruction::RESIZEARR:
      std::dynamic_pointer_cast<JSArray,
        JSBasicObject>(accumulator_.object_)->resize(bytecode.value_.value_);
      Continue();

    case Instruction::POPTOARR:
    {
      auto arr = std::make_shared<AstNode>();
      if (!stack_.empty())
        arr->obj_ = stack_.back().object_;
      else arr->obj_ = std::make_shared<JSBasicObject>();
      std::dynamic_pointer_cast<JSArray,
        JSBasicObject>(accumulator_.object_)->Push(arr);
      stack_.pop_back();
      Continue();
    }

    case Instruction::POPTOOBJ:
    {
      auto object = std::make_shared<AstNode>();
      if (!stack_.empty())
        object->obj_ = stack_.back().object_;
      else object->obj_ = std::make_shared<JSBasicObject>();
      std::dynamic_pointer_cast<JSObject,
        JSBasicObject>(accumulator_.object_)
        ->AddProperty(bytecode.value_.str_, object);
      stack_.pop_back();
      Continue();
    }

    case Instruction::OPOPTARR:
    {
      auto arr = std::make_shared<AstNode>();
      if (!stack_.empty())
        arr->obj_ = stack_.back().object_;
      else arr->obj_ = std::make_shared<JSBasicObject>();
      std::dynamic_pointer_cast<JSArray,
        JSBasicObject>(o_stack_.back().object_)->Push(arr);
      stack_.pop_back();
      Continue();
    }

    case Instruction::OPOPTOBJ:
    {
      auto object = std::make_shared<AstNode>();
      if (!stack_.empty())
        object->obj_ = stack_.back().object_;
      else object->obj_ = std::make_shared<JSBasicObject>();
      std::dynamic_pointer_cast<JSObject,
        JSBasicObject>(o_stack_.back().object_)
        ->AddProperty(bytecode.value_.str_, object);
      stack_.pop_back();
      Continue();
    }

    case Instruction::OPOPTS:
    {
      stack_.push_back(o_stack_.back());
      o_stack_.pop_back();
      Continue();
    }

    case Instruction::POPKEY:
    {
      auto obj = js::heap.FindVariable(bytecode.value_.str_);
      if (!obj.get()
          || obj->expression_type_ == AstNode::ExpressionType::_undefined) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      *(obj->obj_) = *(stack_.back().object_);
      stack_.pop_back();
      Continue();
    }

    case Instruction::POPADR:
      data_[bytecode.value_.value_] = stack_.back();
      stack_.pop_back();
      Continue();

    case Instruction::POPA:
      accumulator_ = stack_.back();
      stack_.pop_back();
      Continue();

    case Instruction::EMPTY:
      if (!stack_.empty())
        stack_.clear();
      Continue();

    case Instruction::ADD:
      status = binary_add();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::SUB:
      status = binary_sub();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::MUL:
      status = binary_mul();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::DIV:
      status = binary_div();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::MOD:
      status = binary_mod();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::SHR:
      status = shift_right();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::SHL:
      status = shift_left();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::CALL:
      handle_call(bytecode);
      Continue();


    case Instruction::EQ:
      status = do_equal();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::NEQ:
      status = do_not_equal();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::LT:
      status = do_less_than();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::GT:
      status = do_greater_than();
      if (status) {
        Continue();
      }
      else {
        StopMachine();
      }

    case Instruction::JMP:
    {
      auto sign = static_cast<int>(bytecode.value_.value_);
      instruction_register_.value_ += sign;
      bytecode = NextInstruction();
      continue;
    }

    case Instruction::JMPZ:
      if (!is_zero(accumulator_)) {
        instruction_register_.value_ += 1;
      }
      Continue();

    case Instruction::RET:
      Continue();
    }
  }
}

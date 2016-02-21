// vm.c

#include "vm.h"

#include <cstdio>
#include <algorithm>
#include <exception>

js::Stack Heap::heap = js::Stack();
#define ObjectHandleType std::shared_ptr<AstNode>
#define NewDefaultHandle() std::make_shared<AstNode>()
#define MakeShared(type, ...) std::make_shared<type>(__VA_ARGS__)
#define NewHandle(object) MakeShared(AstNode, object)
#define Cast(object, To, From) std::dynamic_pointer_cast<To, From>(object)
#define IsArray(object) (object->is<J::_array>())
#define IsNumber(object) (object->is<J::_number>())
#define IsString(object) (object->is<J::_string>())
#define IsObject(object) (object->is<J::_object>())
#define J JSBasicObject::ObjectType

#define NextInstruction()                                     \
  (text_.at((size_t)instruction_register_.value_++))

#define Continue()                                            \
  { bytecode = NextInstruction();                             \
  continue; }

#define StopMachine()                                         \
  bytecode.instruction_ = static_cast<ui>(Instruction::HLT);  \
  continue;

// handle a function call
// first we have to count the number of arguments passed to the function
// and actual parameters required. The minimum of both is calculated and
// this minimum is the actual number of arguments passed
bool handle_call(Machine *machine, Bytecode &bytecode) {
  auto function = std::dynamic_pointer_cast<JSFunction, JSBasicObject>(
    machine->o_stack_.back().handle_->obj_);

  machine->function_stack_.push_back(Register(machine->o_stack_.back()));

  if (!function->is<J::_function>()) {
    printf("NotAnFunctionError\n");
    return false;
  }

  auto parameters = function->GetParameters();
  std::list<std::pair<std::string, std::shared_ptr<AstNode>>> parameter_list;
  auto argument_number = bytecode.value_.value_;
  auto actual_arguments = std::min(parameters.size(), size_t(argument_number));
  
  if (actual_arguments > machine->stack_.size()) {
    printf("InternalError\n");
    return false;
  }
  Heap::heap.CreateScope();
  if (actual_arguments > 0) {
    Register temp;
    std::shared_ptr<AstNode> temp_node;
    auto pit = parameters.end() - 1;
    for (auto i = size_t(0); i < actual_arguments; i++) {
      temp_node = machine->stack_.back().handle_;
      machine->stack_.pop_back();
      Heap::heap.PushVariable({ *(pit - i), temp_node });
    }
  }
  if (function->IsNative()) {
    auto result = handle_call_native(machine, bytecode, function);  
  }
  else {
    Machine mac;
    mac.prepare_fast(&function->function_body_->text_);
    mac.execute();

    // store the returned result
    if (mac.returned_ == true 
        && !mac.stack_.empty()) {
      machine->o_stack_.push_back(mac.stack_.back());
      mac.stack_.pop_back();
    }
  }
  machine->function_stack_.pop_back();
  Heap::heap.RemoveScope();
  return true;
}

// call a native function
bool handle_call_native(Machine *machine,
                        Bytecode &bytecode,
                        std::shared_ptr<JSFunction> function) {
  return function->GetNativeFunction()(machine);
}

bool do_ret(Machine *machine,
            Bytecode &bytecode,
            std::shared_ptr<JSFunction> function) {
  return false;
}

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

J GetType(Register &reg) {
  switch (reg.type_) {
  case DataType::Number:
    return J::_number;

  case DataType::String:
    return J::_string;

  case DataType::Object:
  {
    if (reg.handle_->obj_.get()) {
      return reg.handle_->obj_->GetType();
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
    res = data_[(size_t)registers_[r].value_];
  }
  catch (std::exception e) {
    return Register();
  }
  return res;
}

J decode_type(Register r2, Register r3) {
  auto t1 = r2.handle_->obj_->GetType();
  auto t2 = r3.handle_->obj_->GetType();
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
  data_[(size_t)registers_[(reg_ >> 8) & 0xf].value_] = res;
}

bool Machine::binary_add() {
  Register a, b, res;
  a = stack_.back();
  stack_.pop_back();

  b = stack_.back();
  stack_.pop_back();

  res.handle_ = std::make_shared<AstNode>();
  auto type = ::decode_type(a, b);
  switch (type) {
  case J::_number:
    res.handle_->obj_ = std::dynamic_pointer_cast<JSBasicObject,
      JSNumber>(std::make_shared<JSNumber>(
      a.handle_->obj_->GetNumber() + b.handle_->obj_->GetNumber()));
    res.type_ = DataType::Object;
    break;

  case J::_string:
    res.handle_->obj_ = std::dynamic_pointer_cast<JSBasicObject,
      JSString>(std::make_shared<JSString>(
        a.handle_->obj_->ToString() + b.handle_->obj_->ToString()));
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
  res.handle_ = std::make_shared<AstNode>();                  \
  auto type = ::decode_type(a, b);                            \
  switch (type) {                                             \
  case J::_number:                                            \
    res.handle_->obj_ = std::dynamic_pointer_cast<JSBasicObject,\
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.handle_->obj_->GetNumber()                          \
            op b.handle_->obj_->GetNumber()));                \
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
BINARYOP(binary_and, &&)
BINARYOP(binary_or, ||)
BINARYOP(bit_and, &)
BINARYOP(bit_or, |)
BINARYOP(bit_xor, ^ )

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
  res.handle_ = std::make_shared<AstNode>();                  \
  switch (type) {                                             \
  case J::_number:                                            \
    res.handle_->obj_ = std::dynamic_pointer_cast<JSBasicObject,\
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.handle_->obj_->GetNumber()                          \
                   op b.handle_->obj_->GetNumber()));         \
    res.type_ = DataType::Object;                             \
    break;                                                    \
                                                              \
  case J::_string:                                            \
    res.handle_->obj_ = std::dynamic_pointer_cast<JSBasicObject,\
      JSNumber>(std::make_shared<JSNumber>(                   \
        a.handle_->obj_->ToString() op b.handle_->obj_->ToString()));\
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

bool Machine::do_assign() {
  auto object = o_stack_.back().handle_;
  auto rhs = stack_.back();
  stack_.pop_back();
  object->obj_ = rhs.handle_->obj_;
  return true;
}

bool Machine::is_zero(Register &reg) {
  auto type = GetType(reg);
  switch (type) {
  case J::_number:
    return !reg.handle_->obj_->GetNumber();

  case J::_string:
    return !reg.handle_->obj_->ToString().size();

  default:
    return false;
  }
}

bool Machine::HandleSpecialObject(std::shared_ptr<JSBasicObject> &object,
                                  Bytecode &bytecode) {
  if (object->is<J::_array>()) {
    return false;
  }
  return false;
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
      accumulator_ = data_.at((size_t)bytecode.value_.value_);
      Continue();

    case Instruction::LDAKEY:
    {
      auto obj_ = Heap::heap.FindVariable(bytecode.value_.str_);
      if (!obj_.get()
          || obj_->expression_type_ == AstNode::ExpressionType::_undefined) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      accumulator_ = Register(obj_);
      Continue();
    }

    case Instruction::STRA:
    {
      data_[(size_t)bytecode.value_.value_] = accumulator_;
      Continue();
    }

    case Instruction::PUSHI:
      stack_.push_back(bytecode.value_);
      Continue();

    case Instruction::PUSHADR:
      stack_.push_back(data_[(size_t)bytecode.value_.value_]);
      Continue();

    case Instruction::PUSHREG:
      stack_.push_back(accumulator_);
      Continue();
    
    case Instruction::OPUSHREG:
      o_stack_.push_back(accumulator_);
      Continue();

    case Instruction::PUSHKEY:
    {
      auto obj_ = Heap::heap.FindVariable(bytecode.value_.str_);
      if (!obj_.get()) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      stack_.push_back(Register(obj_));
      Continue();
    }

    case Instruction::OPUSHOBJ:
    {
      ObjectHandleType handle = NewDefaultHandle();
      handle->obj_ = Cast(MakeShared(JSObject), JSBasicObject, JSObject);
      o_stack_.push_back(handle);
      Continue();
    }

    case Instruction::OPUSHARR:
    {
      ObjectHandleType handle = NewDefaultHandle();
      handle->obj_ = Cast(MakeShared(JSArray), JSBasicObject, JSArray);
      o_stack_.push_back(handle);
      Continue();
    }

    case Instruction::OPUSHKEY:
    {
      auto obj = Heap::heap.FindVariable(bytecode.value_.str_);
      if (!obj.get()) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      o_stack_.push_back(Register(obj));
      Continue();
    }

    case Instruction::OPUSHOBJPROP:
    {
      auto &parent_object = o_stack_.back();
      auto real_parent_object = parent_object.handle_->obj_;
      std::shared_ptr<AstNode> object;
      if (IsObject(real_parent_object)) {
        object = Cast(real_parent_object, JSObject, JSBasicObject)
                    ->GetProperty(bytecode.value_.str_);
        if (!object.get()) {
          printf("ReferenceError: Property named %s doesn't exists\n",
                 bytecode.value_.str_.c_str());
          break;
        }
        o_stack_.push_back(object);
      }
      else if (!HandleSpecialObject(real_parent_object, bytecode)) {
        printf("ReferenceError: %s operation not allowed\n",
               bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      Continue();
    }

    case Instruction::OPUSHARROBJ:
    {
      auto &parent_array = o_stack_.back();
      auto real_parent_array = parent_array.handle_->obj_;
      std::shared_ptr<AstNode> object = NewHandle();
      Register temp = stack_.back();
      stack_.pop_back();
      if (IsArray(real_parent_array)) {
        switch (temp.handle_->obj_->GetType()) {
        case J::_number:
          object = Cast(real_parent_array, JSArray, JSBasicObject)->At(
              Cast(temp.handle_->obj_, JSNumber, JSBasicObject)->GetNumber());
          o_stack_.push_back(object);
          break;

        default:
          printf("InvalidArrayIndexError\n");
          status = false;
          break;
        }
      }
      else if (IsObject(real_parent_array)) {
        if (IsString(temp.handle_->obj_)) {
          std::string prop = temp.handle_->obj_->ToString();
          object = Cast(real_parent_array, JSObject, JSBasicObject)
                  ->GetProperty(prop);
          if (!object.get()) {
            printf("%s property doesn't exists\n", prop.c_str());
            status = false;
            break;
          }
          o_stack_.push_back(object);
        }
        else {
          printf("UnknownError: Object property accessed by wrong type\n");
          status = false;
          break;
        }
      }
      else if (IsString(real_parent_array)) {
        switch (temp.handle_->obj_->GetType()) {
        case J::_number:
          object->obj_ = Cast(real_parent_array, JSString, JSBasicObject)
            ->At(Cast(temp.handle_->obj_, JSNumber, JSBasicObject)->GetNumber());
          o_stack_.push_back(object);
          break;

        default:
          printf("InvalidArrayIndexError\n");
          status = false;
          break;
        }
      }
      else {
        printf("NotAnArrayError\n");
        status = false;
        break;
      }
    }
    Continue();

    case Instruction::LDAARR:
      accumulator_ = NewHandle(Cast(MakeShared(JSArray),
                                    JSBasicObject, JSArray));
      Continue();

    case Instruction::LDAOBJ:
      accumulator_ = NewHandle(Cast(MakeShared(JSObject),
                                    JSBasicObject, JSObject));
      Continue();

    case Instruction::LDANUM:
      accumulator_ = NewHandle(Cast(MakeShared(JSNumber),
                                    JSBasicObject, JSNumber));
      Continue();

    case Instruction::LDASTR:
      accumulator_ = NewHandle(Cast(MakeShared(JSString),
                                    JSBasicObject, JSString));
      Continue();

    case Instruction::RESIZEARR:
      std::dynamic_pointer_cast<JSArray,
        JSBasicObject>(accumulator_.handle_->obj_)->resize(
          (size_t)bytecode.value_.value_);
      Continue();

    case Instruction::POPTOARR:
    {
      auto arr = NewHandle();
      if (!stack_.empty())
        arr = stack_.back().handle_;
      else arr->obj_ = std::make_shared<JSBasicObject>();
      Cast(accumulator_.handle_->obj_, JSArray, JSBasicObject)->Push(arr);
      stack_.pop_back();
      Continue();
    }

    case Instruction::POPTOOBJ:
    {
      auto object = NewHandle();
      if (!stack_.empty())
        object = stack_.back().handle_;
      else object->obj_ = std::make_shared<JSBasicObject>();
      Cast(accumulator_.handle_->obj_, JSObject,
        JSBasicObject)->AddProperty(bytecode.value_.str_, object);
      stack_.pop_back();
      Continue();
    }

    case Instruction::OPOPTARR:
    {
      auto arr = NewHandle();
      if (!stack_.empty())
        arr = stack_.back().handle_;
      else arr->obj_ = std::make_shared<JSBasicObject>();
      Cast(o_stack_.back().handle_->obj_, JSArray, JSBasicObject)->Push(arr);
      stack_.pop_back();
      Continue();
    }

    case Instruction::OPOPTOBJ:
    {
      auto object = NewHandle();
      if (!stack_.empty())
        object = stack_.back().handle_;
      else object->obj_ = std::make_shared<JSBasicObject>();
      Cast(o_stack_.back().handle_->obj_, JSObject, JSBasicObject)
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

    case Instruction::OASSIGN:
    {
      do_assign();
      Continue();
    }

    case Instruction::POPKEY:
    {
      auto obj = Heap::heap.FindVariable(bytecode.value_.str_);
      if (!obj.get()) {
        printf("ReferenceError: the variable named %s"
               " doesn't exists\n", bytecode.value_.str_.c_str());
        status = false;
        break;
      }
      (obj->obj_) = (stack_.back().handle_->obj_);
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

    case Instruction::INC:
    {
      auto &maybenumber = stack_.back();
      if (IsNumber(maybenumber.handle_->obj_)) {
        auto number = Cast(maybenumber.handle_->obj_, JSNumber, JSBasicObject);
        number->GetNumber()++;
        Continue();
      }
      else {
        printf("NanError: the value was not an integer");
        status = false;
        break;
      }
    }
    
    case Instruction::DEC:
    {
      auto &maybenumber = stack_.back();
      if (IsNumber(maybenumber.handle_->obj_)) {
        auto number = Cast(maybenumber.handle_->obj_, JSNumber, JSBasicObject);
        number->GetNumber()--;
        Continue();
      }
      else {
        printf("NanError: the value was not an integer");
        status = false;
        break;
      }
    }

    case Instruction::PINC:
    {
      auto maybenumber = stack_.back();
      stack_.pop_back();      // pop the element because we don't need it
      if (IsNumber(maybenumber.handle_->obj_)) {
        auto number = Cast(maybenumber.handle_->obj_, JSNumber, JSBasicObject);
        auto tobestacked = MakeShared(JSNumber, number->GetNumber()++);
        auto handle = NewHandle(tobestacked);
        auto reg = Register(handle);
        stack_.push_back(reg);
        Continue();
      }
      else {
        printf("NanError: the value was not an integer");
        status = false;
        break;
      }
    }

    case Instruction::PDEC:
    {
      auto maybenumber = stack_.back();
      stack_.pop_back();      // pop the element because we don't need it
      if (IsNumber(maybenumber.handle_->obj_)) {
        auto number = Cast(maybenumber.handle_->obj_, JSNumber, JSBasicObject);
        auto tobestacked = MakeShared(JSNumber, number->GetNumber()--);
        auto handle = NewHandle(tobestacked);
        auto reg = Register(handle);
        stack_.push_back(reg);
        Continue();
      }
      else {
        printf("NanError: the value was not an integer");
        status = false;
        break;
      }
    }

    case Instruction::NEG:
    {
      auto maybenumber = stack_.back();
      stack_.pop_back();      // pop the element because we don't need it
      if (IsNumber(maybenumber.handle_->obj_)) {
        auto number = Cast(maybenumber.handle_->obj_, JSNumber, JSBasicObject);
        auto tobestacked = MakeShared(JSNumber, -number->GetNumber());
        auto handle = NewHandle(tobestacked);
        auto reg = Register(handle);
        stack_.push_back(reg);
        Continue();
      }
      else {
        printf("NanError: the value was not an integer");
        status = false;
        break;
      }
    }

    case Instruction::ADD:
      status = binary_add();
      Continue();

    case Instruction::SUB:
      status = binary_sub();
      Continue();

    case Instruction::MUL:
      status = binary_mul();
      Continue();

    case Instruction::DIV:
      status = binary_div();
      Continue();

    case Instruction::MOD:
      status = binary_mod();
      Continue();

    case Instruction::SHR:
      status = shift_right();
      Continue();

    case Instruction::SHL:
      status = shift_left();
      Continue();

    case Instruction::CALL:
      handle_call(this, bytecode);
      Continue();


    case Instruction::EQ:
      status = do_equal();
      Continue();

    case Instruction::NEQ:
      status = do_not_equal();
      Continue();

    case Instruction::LT:
      status = do_less_than();
      Continue();

    case Instruction::GT:
      status = do_greater_than();
      Continue();

    case Instruction::BITOR:
      status = bit_or();
      Continue();

    case Instruction::BITXOR:
      status = bit_xor();
      Continue();

    case Instruction::BITAND:
      status = bit_and();
      Continue();

    case Instruction::AND:
      status = binary_and();
      Continue();

    case Instruction::OR:
      status = binary_or();
      Continue();

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

    case Instruction::RET0:
      return;

    case Instruction::RET:
      returned_ = true;
      return;


    default:
      printf("FatalError: The instruction doesn't exists\n");
      status = false;
    }
  }
  return;
}

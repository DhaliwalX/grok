#ifndef VIRTUAL_MACHINE_H_
#define VIRTUAL_MACHINE_H_

#include "bytecode.h"
#include "register.h"
#include "program.h"
#include "heap.h"
#include "jsobject.h"
#include "jsarray.h"
#include "jsfunction.h"
#include "object.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include "list.h"
#include <list>
#include <vector>
#include <map>

enum class ScriptError : short {

};

namespace js {
static Stack heap;
}

class Machine {
public:
  Machine() :
    instruction_executed_(0),
    instruction_register_(),
    accumulator_(),
    program_(nullptr),
    data_(),
    text_(),
    stack_() {
  }

  void prepare_machine(BytecodeProgram<Register, Bytecode> *program) {
    program_ = program;
    instruction_executed_ = 0;
    if (text_.size())
      program_start_.set(text_.size());
    else program_start_ = 0;
    data_.insert(data_.end(), program->data_.begin(), program->data_.end());
    text_.insert(text_.end(), program->text_.begin(), program->text_.end());
  }

  inline Register accumulator() { return data_[accumulator_.value_]; }

  void execute();

  inline bool shift_left();
  inline bool shift_right();
  inline bool binary_add();
  inline bool binary_sub();
  inline bool binary_mul();
  inline bool binary_div();
  inline bool binary_mod();
  inline bool do_equal();
  inline bool do_not_equal();
  inline bool do_less_than();
  inline bool do_greater_than();

  inline std::pair<unsigned long long, bool>
    effective_address(Bytecode & code);

  inline void report_error(const std::string&);
  inline void script_error(const std::string&);
  inline void print_stack_trace();
  inline void decode_registers(int r, int&, int&, int&);
  inline bool check_if_valid(int, int, int);
  inline Register calculate_operand(int r);

  inline void set_object(Register &res);

  inline bool is_zero(Register&);
  inline bool handle_call(Bytecode &);

  inline std::string dump() {
    std::ostringstream str;
    str << "Instructions executed: " << instruction_executed_;
    str << "\nIR: ";
    instruction_register_.print(str);
    for (int i = 0; i < 8; i++) {
      str << "\nR" << i << ": ";
      registers_[i].print(str);
    }
    str << "\n";
    str << "AC: ";
    accumulator_.print(str);
    str << "\n";
    str << "Stack: \n";
    for (auto p : stack_) {
      p.print(str);
      str << "\n";
    }

    str << "Globals:\n";
    for (auto p : data_) {
      p.print(str);
    }
    return str.str();
  }

  Register StackTop() {
    if (!stack_.empty())
      return stack_.back();
    return Register();
  }

  // tracks the number of instruction being executed
  // will be enabled in only debug mode
  unsigned long long instruction_executed_;

  // stores the current instruction being executed
  Register instruction_register_;

  Register registers_[8];

  // accumulator
  Register accumulator_;

  Register idx_;

  // contains the pointer to the first instruction of the current program
  Register program_start_;

  // pointer to the current program
  BytecodeProgram<Register, Bytecode> *program_;

  // data section of all the programs
  std::vector<Register> data_;

  // text section of all the programs
  std::vector<Bytecode> text_;

  // general purpose stack
  std::list<Register> stack_;

  // stack for object storage
  std::list<Register> o_stack_;

  // current instruction's register specs
  unsigned int reg_;
  
  js::VStack heap2_;
};

#endif // VIRTUAL_MACHINE_H_

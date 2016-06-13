#ifndef REGISTER_H_
#define REGISTER_H_

#include <iostream>
#include <string>
#include <memory>

#include "astnode.h"

enum class InstructionKind : unsigned short { None, Imm, Adr, Reg, Key, Idx };

enum class DataType : unsigned short {
  Undefined,
  Nan,
  Number,
  String,
  Object,
  Array
};

static std::string instruction_kinds[] = {"None", "Imm", "Adr",
                                          "Reg",  "Key", "Count"};

// register class
class Register {
public:
  Register() : value_(0), type_(DataType::Undefined) {}

  Register(long long value) : value_(value), type_(DataType::Number) {}

  Register(const std::string &str)
      : str_(str), type_(DataType::String), value_(0) {}

  Register(std::shared_ptr<AstNode> obj)
      : handle_(obj), value_(0), type_(DataType::Object) {}

  Register(const Register &reg)
      : value_(reg.value_), str_(reg.str_), type_(reg.type_),
        handle_(reg.handle_) {}

  Register &operator=(const Register &reg) {
    value_ = (reg.value_);
    str_ = reg.str_;
    type_ = reg.type_;
    handle_ = reg.handle_;
    return *this;
  }

  Register &operator=(const std::string &reg) {
    str_ = reg;
    type_ = DataType::String;
    return *this;
  }

  Register &operator=(std::shared_ptr<AstNode> &object) {
    handle_ = object;
    type_ = DataType::Object;
    return *this;
  }

  Register &operator=(long long value) {
    value_ = value;
    type_ = DataType::Number;
    return *this;
  }

  ~Register() = default;

  long long &Value() { return this->value_; }

  std::string &String() { return this->str_; }

  void set(long long value) {
    this->value_ = value;
    type_ = DataType::Number;
  }

  void set(const std::string &str) {
    this->str_ = str;
    type_ = DataType::String;
  }

  void set(long long address, unsigned long size) {
    this->value_ = address;
    str_ = "";
  }

  void reset() {
    this->value_ = 0;
    this->str_ = "";
  }

  void print(std::ostream &os) const {
    switch (this->type_) {
    case DataType::Number:
      os << this->value_;
      break;

    case DataType::String:
      os << this->str_;
      break;

    case DataType::Undefined:
      break;

    case DataType::Array:
      os << this->handle_->obj_->ToString();
      break;

    case DataType::Object:
      os << this->handle_->obj_->ToString();
      break;
    }
    return;
  }

  long long value_;
  std::string str_;
  DataType type_;
  std::shared_ptr<AstNode> handle_;
};

template <typename data_t> class RegisterHeavy {
public:
  RegisterHeavy() : data_(nullptr) {}

  RegisterHeavy(data_t data) : data_(data) {}

  RegisterHeavy(const RegisterHeavy &reg) : data_(reg.data_) {}

  RegisterHeavy &operator=(const RegisterHeavy &reg) {
    data_ = reg.data_;
    return *this;
  }

  ~RegisterHeavy() = default;

  data_t data_;
};

#endif

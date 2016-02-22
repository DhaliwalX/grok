#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <vector>
#include <iostream>

// A program contains the bytecodes to execute and data
// A program contains the data section and instruction section
// A vector contains all the data section and another vector contains the
// instructions of the program
template <typename DataType, typename BytecodeType> class BytecodeProgram {
public:
  BytecodeProgram() : data_(), text_() {}

  BytecodeProgram(std::vector<DataType> data, std::vector<BytecodeType> text)
      : data_(data), text_(text) {}

  BytecodeProgram(const BytecodeProgram<DataType, BytecodeType> &program)
      : data_(program.data_), text_(program.text_) {}

  BytecodeProgram &
  operator=(const BytecodeProgram<DataType, BytecodeType> &program) {
    data_ = program.data_;
    text_ = program.text_;
    return *this;
  }

  void print(std::ostream &os) {
    os << "--program--\n";
    os << "Instructions: " << text_.size() << "\n";
    os << ".data\n";
    for (auto each : data_) {
      os << "\t";
      each.print(os);
      os << "\n";
    }
    os << ".text\n";
    for (auto each : text_) {
      os << "\t";
      each.print(os);
      os << "\n";
    }
  }

  ~BytecodeProgram() = default;

  std::vector<DataType> &Data() { return this->data_; }
  std::vector<BytecodeType> &Text() { return this->text_; }

  std::vector<DataType> data_;
  std::vector<BytecodeType> text_;
};

#endif // BYTECODE_LIST_H_

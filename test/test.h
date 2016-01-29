#include "../src/bytecode.h"
#include "../src/symtable.h"
#include "../src/vm.h"
#include "../src/timer.h"
#include "../src/grok.h"
#include "../src/codegen.h"

#include "test-lexer.h"
namespace test {

// test the Bytecode class
int test1() {
  Bytecode bytecode{ Instruction::LDAI, Register() };
  bytecode.print(std::cout);
  return 0;
}

int test2() {
  SymbolTable<int> table;
  table["prince"] = 322;
  table["one"] = 1;
  table["two"] = 2;
  table["three"] = 3;

  for (auto p : table) {
    std::cout << p.first << ": " << p.second;
  }
  std::cout << "\n";
  return 0;
}

int test3() {
  BytecodeProgram<Register, Bytecode> *program =
    new BytecodeProgram<Register, Bytecode>();

  using namespace B;
  std::cout << "\nCreating data section\n";
  // create .data section
  program->Data().push_back(Register(1));
  program->Data().push_back(Register(2));
  program->Data().push_back(Register(1000000));

  Machine *machine = new Machine();
  std::cout << "\nPreparing MAchine\n";
  machine->prepare_machine(program);
  std::cout << "Executing\n";
  Timer time;
  time.Start();
  machine->execute();
  time.Stop();
  time.PrintUS();
  auto result = program->Data()[0];
  std::cout << result.value_ << std::endl;
  std::cout << machine->dump() << std::endl;
  return 0;
}

void test7() {
  while (true) {
    std::shared_ptr<AstNode> node;
    std::string expr;
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    Expression::Parse(lex, node);
    Code::CodeGen codegen;
    std::cout << "Generating parse tree: \n";
    PrintAST(node, std::cout);
    BytecodeProgram<Register, Bytecode> *program =
      new BytecodeProgram<Register, Bytecode>();
    std::cout << "Generating code: \n";
    //codegen.GenerateCode(program, node);
    Machine machine;
    codegen.GenerateCode(program, node->links_[0], &machine);
    program->text_.push_back(B::hlt());
    program->print(std::cout);
    std::cout << "\nExecuting...\n";
    machine.prepare_machine(program);
    Timer time;
    time.Start();
    machine.execute();
    time.Stop();
    time.PrintUS();
    std::cout << machine.dump();
    delete lex;
    delete program;
  }
}

void test4() {
  while (true) {
    std::shared_ptr<AstNode> node;
    std::string expr;
    SYM::symbol_table.insert({ std::string("x"), 0 });
    SYM::symbol_table.insert({ std::string("y"), 1 });
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    Expression::Parse(lex, node);
    Code::CodeGen codegen;
    BytecodeProgram<Register, Bytecode> *program =
      new BytecodeProgram<Register, Bytecode>();
    std::cout << "Generating code: \n";
    //codegen.GenerateCode(program, node);
    Machine machine;
    Code::GenerateIf(&codegen, program, node->links_[0], &machine);
    program->data_.push_back(Register(123));
    program->data_.push_back(Register(444));
    program->text_.push_back(B::hlt());
    program->print(std::cout);
    std::cout << "\nExecuting...\n";
    machine.prepare_machine(program);
    Timer time;
    time.Start();
    machine.execute();
    time.Stop();
    time.PrintUS();
    std::cout << machine.dump();
  }
}

void test8() {
  std::ofstream out("syntax.ast");
  while (true) {
    out << "-------------------------------------------------------------------\n";
    std::shared_ptr<AstNode> node;
    std::string expr;
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    bool result = Expression::Parse(lex, node);
    PrintAST(node, std::cout);
    PrintAST(node, out);
    std::cout << "\n" << std::boolalpha << result << std::endl;
    delete lex;
  }
}

void test10() {
  {
    std::ofstream out("syntax.ast");
    Timer t;
    std::shared_ptr<AstNode> node;
    Lexer *lex = new Lexer("tmp.js", true);
    t.Start();
    bool result = Expression::Parse(lex, node);
    t.Stop();
    t.PrintMS();
    PrintAST(node, out);
    std::cout << "\n" << std::boolalpha << result << std::endl;
    delete lex;
  }
}
void test5() {
  while (true) {
    std::shared_ptr<AstNode> node;
    std::string expr;
    SYM::symbol_table.insert({ std::string("x"), 0 });
    SYM::symbol_table.insert({ std::string("y"), 1 });
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    Expression::Parse(lex, node);
    Code::CodeGen codegen;
    BytecodeProgram<Register, Bytecode> *program =
      new BytecodeProgram<Register, Bytecode>();
    std::cout << "Generating code: \n";
    //codegen.GenerateCode(program, node);
    Machine machine;
    Code::GenerateWhile(&codegen, program, node->links_[0], &machine);
    program->data_.push_back(Register(123));
    program->data_.push_back(Register(444));
    program->text_.push_back(B::hlt());
    program->print(std::cout);
    std::cout << "\nExecuting...\n";
    machine.prepare_machine(program);
    Timer time;
    time.Start();
    machine.execute();
    time.Stop();
    time.PrintUS();
   // printf("%s\n>> ", machine.dump().c_str());
  }
}

void test6() {
  while (true) {
    std::shared_ptr<AstNode> node;
    std::string expr;
    SYM::symbol_table.insert({ std::string("x"), 0 });
    SYM::symbol_table.insert({ std::string("y"), 1 });
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    Expression::Parse(lex, node);
    Code::CodeGen codegen;
    BytecodeProgram<Register, Bytecode> *program =
      new BytecodeProgram<Register, Bytecode>();
    std::cout << "Generating code: \n";
    //codegen.GenerateCode(program, node);
    Machine machine;
    Code::GenerateFor(&codegen, program, node->links_[0], &machine);
    program->data_.push_back(Register(123));
    program->data_.push_back(Register(444));
    program->text_.push_back(B::hlt());
    program->print(std::cout);
    std::cout << "\nExecuting...\n";
    machine.prepare_machine(program);
    Timer time;
    time.Start();
    machine.execute();
    time.Stop();
    time.PrintUS();
    printf("%s\n>> ", machine.dump().c_str());
  }
}

void test9() {
  std::shared_ptr<AstNode> node;
  std::string expr;
  Lexer *lex = nullptr;
  Code::CodeGen codegen;
  Machine *machine = new Machine();
  Timer time;
  BytecodeProgram<Register, Bytecode> *program = nullptr;
  while (true) {
    std::printf(">> ");
    std::getline(std::cin, expr);
    MakeLexer(&lex, expr);
    if (!Expression::Parse(lex, node)) {
      printf("%s\n", Expression::err_msg_.c_str());
      continue;
    }
    SYM::symbol_table.clear();
    program = new BytecodeProgram<Register, Bytecode>();
    for (auto &child : node->links_)
      if (!Code::GenerateCode(&codegen, program, child, machine)) {
        printf("Code Generation Failed\n");
        continue;
      }
    program->text_.push_back(B::hlt());
    //program->print(std::cout);
    machine->prepare_machine(program);
    time.Start();
    machine->execute();
    time.Stop();
    //time.PrintUS();
    //printf("%s\n ", machine->dump().c_str());
  
  backup:
    delete program;
  }
  delete lex;
  delete machine;

}

int testmain() {
 // test10();
  test8();
  test9();
  test7();
  test6();
  test5();
  test3();
  std::cin.get();
  return 0;
}


} // namespace test
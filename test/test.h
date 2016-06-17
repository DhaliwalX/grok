#include "../src/bytecode.h"
#include "../src/symtable.h"
#include "../src/timer.h"
#include "../src/grok.h"

namespace test {

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


void test8() {
  while (true) {
    std::shared_ptr<AstNode> node;
    std::string expr;
    std::cout << ">> ";
    std::getline(std::cin, expr);
    Lexer *lex = new Lexer(expr);
    bool result = Expression::Parse(lex, node);
    PrintAST(node, std::cout);
    std::cout << "\n" << std::boolalpha << result << std::endl;
    delete lex;
  }
}

int testmain() {
  test8();
  std::cin.get();
  return 0;
}


} // namespace test
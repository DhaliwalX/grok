#include "../src/symtable.h"
#include "../src/timer.h"
#include "../src/grok.h"

namespace test {


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
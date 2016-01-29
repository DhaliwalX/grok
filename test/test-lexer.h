
#include "../src/lexer.h"
#include "../src/token.h"

#include <cstdio>


#define TEST_INC()  test_number++
#define TEST_INIT(name) printf("Testing "#name"\n")
#define TEST_SUCCESS(name)  \
  printf("<Test %d> :: %s\n", TEST_INC(), (name() ? "<Ok>" : "<NotOk>"))

namespace test {
int test_number = 0;

int testl1() {
  Lexer lexer = Lexer(std::string()); //empty string test
  auto tok = lexer.NextToken();
  if (tok->Type() != EOS) {
    delete tok;
    return 0;
  }
  delete tok;
  return 1;
}

#define LEX_TEST_INIT() Token* tok
#define NEXT_TOK()    \
  tok = lexer.NextToken()
#define CHECK_TOK(type) \
  if (tok->Type() != type) {   \
    delete tok;               \
    return 0;                 \
  }                           \
  delete tok;                 \
  NEXT_TOK()

#define CREATE_LEXER(str) \
  Lexer lexer = Lexer(str)

#define TEST_END()  return 1;

int testl2() {
  LEX_TEST_INIT();
  CREATE_LEXER(std::string("var x = 10"));
  NEXT_TOK();
  CHECK_TOK(VAR);
  CHECK_TOK(IDENT);
  CHECK_TOK(ASSIGN);
  CHECK_TOK(DIGIT);
  TEST_END();
}

int testl3() {
  LEX_TEST_INIT();
  CREATE_LEXER(std::string("var x = [1, 3, 5];"));
  NEXT_TOK();
  CHECK_TOK(VAR);
  CHECK_TOK(IDENT);
  CHECK_TOK(ASSIGN);
  CHECK_TOK(LSQB);
  CHECK_TOK(DIGIT);
  CHECK_TOK(COMMA);
  CHECK_TOK(DIGIT);
  CHECK_TOK(COMMA);
  CHECK_TOK(DIGIT);
  CHECK_TOK(RSQB);
  CHECK_TOK(SCOLON);
  TEST_END();
}

int test11() {
  TEST_INIT(Lexer);
  TEST_SUCCESS(testl1);
  TEST_SUCCESS(testl2);
  TEST_SUCCESS(testl3);
  return 1;
}

}
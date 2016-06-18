#include "lexer.h"

void MakeLexer(Lexer **lex, std::string &str) {
  if (!*lex) {
    *lex = new Lexer(str);
    return;
  }
  (*lex)->code_ = str;
  (*lex)->seek_ = 0;
  (*lex)->end_ = str.size() - 1;
  (*lex)->tok_ = nullptr;
  (*lex)->eos_ = 0;
  (*lex)->size_ = str.size();
  (*lex)->position_ = Position();
  (*lex)->lastColNumber_ = 0;
  (*lex)->file_name_ = "<stdin>";
  (*lex)->file_ = false;
}

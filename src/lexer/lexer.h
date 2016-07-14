#ifndef LEXER_H_
#define LEXER_H_

#include "lexer/token.h"

#include <string>

#define ERROR(source, arg) source(arg)
#define EX(source, arg1, arg2) throw source(arg1, arg2)

class Lexer;
extern void MakeLexer(Lexer **lex, std::string &str);
// class Lexer : this class takes input as a string and converts
// the string into stream of tokens. It can also be passed a file
// name
class Lexer {
  friend void MakeLexer(Lexer **lex, std::string &str);

public:
  // constructor taking string
  Lexer(const std::string &str)
      : code_(str), seek_(0), end_(str.size() - 1), tok_(), eos_(0),
        size_(str.size()), position_(), lastColNumber_(0)
  { }

  // default constructor
  Lexer() : code_(), seek_(0), end_(0), tok_(), eos_(0), size_(0) {}

  // do nothing destructor
  ~Lexer() {}

  // returns next token after parsing from the
  // current position, can also return token
  // if existed in the buffer
  Token NextToken();

  char LookAhead() { // returns the next character in the string
    return code_[seek_];
  }

  TokenType peek() { return tok_.type(); }

  double GetNumber();
  std::string GetIdentifierName() { return tok_.GetValue(); }
  void advance();

  void StripComments(char &ch);

  void GoBack();

  char NextCharacter();

  Token Characterize(char ch, Position &pos);

  Token ParseNumber(char ch, Position &pos);

  Token ParseIdentifierOrKeyWord(char ch, Position &pos);

  // although not required because of no use
  Token ParseStringLiteral(char ch, Position &pos);

  // this function should be used instead of above function
  // when we found a token of " or ' that means we are about
  // to parse the string from the current position
  std::string GetStringLiteral();

  TokenType OneCharacterSymbol(char ch);
  TokenType TwoCharacterSymbol(char ch1, char ch2);

  TokenType ThreeCharacterSymbol(char ch1, char ch2, char ch3);

  int GetPrecedance() const
  {
    return tok_.precedance_;
  }

  Position GetCurrentPosition() const { return tok_.position_; }

  bool Eos() { return eos_; }

  std::string &GetStringCache()
  {
    return code_;
  }

  size_t GetSeek()
  {
    return seek_;
  }
private:
  std::string code_; // whole code will stored here
  size_t seek_;         // current position of the seek
  size_t end_;
  Token tok_;       // buffer required for PutBack()
  bool eos_;     // end of file flag
  size_t size_; // size_ of the string code_
  Position position_;
  int lastColNumber_; // current position in terms of line no and column no.
  int save_end_;
  short status_; // status of the lexer
};

#endif

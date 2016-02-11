///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Pushpinder Singh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//  \file:  lexer.h
//  \description:  definition of Lexer class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LEXER_H_
#define LEXER_H_

#include "token.h"

#include <fstream>
#include <iostream>
#include <string>
#include <list>

#define ERROR(source, arg) source(arg)
#define EX(source, arg1, arg2) throw source(arg1, arg2)

class Lexer;
static void MakeLexer(Lexer **lex, std::string &str);
// class Lexer : this class takes input as a string and converts
// the string into stream of tokens. It can also be passed a file
// name
class Lexer {
  friend void MakeLexer(Lexer **lex, std::string &str);
public:

  // constructor taking string
  Lexer(const std::string &str) :
    code_(str), seek_(0), end_(str.size() - 1), tok_(nullptr), eos_(0),
    size_(str.size()), position_(), lastColNumber_(0),
    file_name_("<stdin>"), file_(false)
  { }

  // constructor taking filename
  Lexer(const char *file, bool f)
  {
    file_ = true;
    std::ifstream infile(file);
    file_name_ = file;
    int length = 0, start = 0;
    if (infile.fail()) {
      ERROR(std::string, "cannot open file");
      f = false;
      err_msg_ = file + std::string(": file doesn't exist\n");
      return;
    }
    infile.seekg(0, infile.end);
    length = (size_t)infile.tellg();
    infile.seekg(0, infile.beg);
    code_.resize(length + 2, ' ');
    infile.read(&*(code_.begin()), length);
    infile.close();
    size_ = length;
    end_ = length;
    seek_ = 0;
    tok_ = nullptr;
    eos_ = false;
  }

  // default constructor
  Lexer() :
    code_(), seek_(0), end_(0), size_(0), tok_(nullptr), eos_(0)
  { }

  // do nothing destructor
  ~Lexer()
  { }

  // returns next token after parsing from the
  // current position, can also return token
  // if existed in the buffer
  Token* NextToken()
  {
    if (!buffer_.empty()) {
      Token *tok = buffer_.back();
      buffer_.pop_back();
      return tok;
    }

    char ch = NextCharacter(), last = ' ';
    // skip whitespaces
    while (ch != EOF && (ch == ' ' || ch == '\n' || ch == '\t'))
      ch = NextCharacter();

    // skip comments
    while (ch != EOF && (ch == '/' && LookAhead() == '/'
      || ch == '/' && LookAhead() == '*'))
      StripComments(ch);

    // note the current position as we are about to parse
    // a valid(may be) token from this position
    Position pos = position_;

    if (ch != EOF) {

      // handle keywords, identifiers and numbers
      while (isalnum(ch) || ch == '_')
        return Characterize(ch, pos);

      if (LookAhead() == EOF
          || LookAhead() == '\0'
          || LookAhead() == ' '
          || LookAhead() == '\t'
          || LookAhead() == '\n')
        goto one;
      // else ch is a symbol
      char ch2 = NextCharacter();
      
      if (LookAhead() == EOF
          || LookAhead() == '\0'
          || LookAhead() == ' '
          || LookAhead() == '\t'
          || LookAhead() == '\n')
        goto two;
      char ch3 = NextCharacter();

      // check if there is a three character symbol
      TokenType tok = ThreeCharacterSymbol(ch, ch2, ch3);
      if (tok != INVALID) {
        return new Token(
          std::string({ ch, ch2, ch3 }), tok, TOKENS[tok].precedance_, pos);
      }
      // as we have gone ahead one character so time to go back
      GoBack();
    two:
      // so now check whether we are at position of two character symbol
      tok = TwoCharacterSymbol(ch, ch2);
      if (tok != INVALID) {
        return new Token(
          std::string({ ch, ch2 }), tok, TOKENS[tok].precedance_, pos);
      }

      // No! Let's go back one character
      GoBack();

    one:
      // Only possibility left
      tok = OneCharacterSymbol(ch);
      if (tok != INVALID) {
        return new Token(std::string({ ch }),
          tok, TOKENS[tok].precedance_, pos);
      }
      else    // oops! something went wrong, token is invalid!
        return new Token(std::string({ ch }), INVALID, 0, pos);
    }

    // finally reached at the end of the given string
    return new Token(std::string({ ch }), EOS, 0, pos);
  }

  char LookAhead()
  { // returns the next character in the string
    return code_[seek_];
  }

  void StripComments(char &ch)
  { // skips all the comments from the string
    char last = '\0';
    // skip the comments - single line comment
    if (ch != EOF && (ch == '/' && LookAhead() == '/')) {
      ch = NextCharacter();
      while (ch != '\n' || ch == EOF)
        ch = NextCharacter();
      ch = NextCharacter();
    }

    // multiline comment, /* */ type
    if (ch != EOF && (ch == '/' && LookAhead() == '*')) {
      last = ch;
      ch = NextCharacter();
      while (ch != EOF && !(last == '*' && ch == '/')) {
        last = ch;
        ch = NextCharacter();
      }
      ch = NextCharacter();
    }

    while (ch != EOF && (ch == ' ' || ch == '\n' || ch == '\t'))
      ch = NextCharacter();
  }

  void GoBack()
  { // go back one character back
    --seek_;
    if (position_.col_ - 1 < 0)
    { // we don't want our col_ to be negative
      position_.row_--;     // needed
      position_.col_ = lastColNumber_;
    }
    else
    { // okay, so simply decrement col_ member
      position_.col_--;
    }
  }

  char NextCharacter()
  { // returns the next character from the string
    if (seek_ <= end_)
    { // if eos_ flag has not been set
      char ch = code_[seek_++];
      if (ch == '\0')
      { // if we are not processing EOF file character
        ch = EOF;
        eos_ = true;
      }
      if (ch == '\n')
      { // change the position
        position_.row_++;
        lastColNumber_ = position_.col_;
        position_.col_ = 0;
      }
      else {
        position_.col_++;
      }
      return ch;
    }
    return EOF;
  }

  Token* Characterize(char ch, Position &pos)
  { // characterizes the token being parsed as a
    // number, keyword, or a identifier

    // check whether it is a identifier
    int i = 0, dot = 0;
    std::string buffer;
    if (isdigit(ch))
      return ParseNumber(ch, pos);

    if (isalpha(ch) || ch == '_' || ch == '$')
      return ParseIdentifierOrKeyWord(ch, pos);

    // if none of both then we have to return a invalid token
    return new Token(std::string(""), INVALID, 0, pos);
  }

  Token* ParseNumber(char ch, Position &pos)
  { // parses the number from the current position
    std::string buffer; // buffer to store the parsed number
    while (isdigit(ch)) {
      buffer += ch;
      ch = NextCharacter();
    }
    // possible floating point number
    if (ch == '.' || ch == 'e' || ch == 'E') {
      buffer += ch;
      ch = NextCharacter();
      while (isdigit(ch)) {
        buffer += ch;
        ch = NextCharacter();
      }
    }
    // we've gone one character ahead, so go back
    GoBack();

    return new Token(buffer, DIGIT, 0, pos);
  }

  Token* ParseIdentifierOrKeyWord(char ch, Position &pos)
  { // parses the identifier or a keyword from the current position
    std::string buffer;
    // possibly it is an identifier
    while (isalnum(ch) || ch == '_' || ch == '$') {
      buffer += ch;
      ch = NextCharacter();
    }
    // again went a character ahead
    GoBack();
    // check for KeyWord
    if (buffer[0] != '_' || buffer[0] != '$') {
      for (int i = LET; i <= RET; i++)
        if (buffer[0] == TOKENS[i].value_[0]
          && buffer == TOKENS[i].value_)
          return new Token(buffer, i, 0, pos);
    }
    return new Token(buffer, IDENT, 0, pos);
  }

  // although not required because of no use
  Token* ParseStringLiteral(char ch, Position &pos)
  { // parses the string literal from the string
    std::string str = "";
    if (ch == '"')
    { // string surrounded by "
      ch = NextCharacter();
      while (ch != '"') {
        str += ch;
        ch = NextCharacter();
      }
    }
    else if (ch == '\'')
    { // string surrounded by '
      ch = NextCharacter();
      while (ch != '\'') {
        str += ch;
        ch = NextCharacter();
      }
    }
    return new Token(str, STRING, 0, pos);
  }

  // this function should be used instead of above function
  // when we found a token of " or ' that means we are about
  // to parse the string from the current position
  std::string GetStringLiteral()
  { // returns all the string until it finds "

    // create a buffer
    std::string str = "";
    char ch = NextCharacter();
    while (ch != EOF && (ch != '"' && ch != '\'')) {
      str += ch;    // fill the buffer
      ch = NextCharacter();
    }
    return str;
  }

  TokenType OneCharacterSymbol(char ch)
  { // returns the type of token from the given symbol
    switch (ch) {
    case '(':
      return LPAR;
    case ')':
      return RPAR;
    case '{':
      return LBRACE;
    case '}':
      return RBRACE;
    case '[':
      return LSQB;
    case ']':
      return RSQB;
    case ':':
      return COLON;
    case ';':
      return SCOLON;
    case '"':
    case '\'':
      return STRING;
    case '.':
      return DOT;
    case ',':
      return COMMA;
    case '?':
      return CONDITION;
    case '+':
      return PLUS;
    case '-':
      return MINUS;
    case '/':
      return DIV;
    case '*':
      return MUL;
    case '%':
      return MOD;
    case '>':
      return GT;
    case '<':
      return LT;
    case '=':
      return ASSIGN;
    case '^':
      return XOR;
    case '|':
      return BOR;
    case '&':
      return BAND;
    case '!':
      return NOT;
    case '~':
      return BNOT;
    default:
      return INVALID;
    }
  }

  TokenType TwoCharacterSymbol(char ch1, char ch2)
  { // returns the type of symbol of two characters
    switch (ch2) {
    case '=':
      switch (ch1) {
      case '+':
        return PLUSEQ;
      case '-':
        return MINUSEQ;
      case '*':
        return MULEQ;
      case '/':
        return DIVEQ;
      case '%':
        return MODEQ;
      case '^':
        return XOREQ;
      case '&':
        return BANDEQ;
      case '|':
        return BOREQ;
      case '>':
        return GTE;
      case '<':
        return LTE;
      case '=':
        return EQUAL;
      case '!':
        return NOTEQ;
      }
      return INVALID;
    case '|':
      if (ch1 == '|')
        return OR;
      return INVALID;
    case '&':
      if (ch1 == '&')
        return AND;
      return INVALID;
    case '+':
      if (ch1 == '+')
        return INC;
      return INVALID;
    case '-':
      if (ch1 == '-')
        return DEC;
      return INVALID;
    case '<':
      if (ch1 == '<')
        return SHL;
      return INVALID;
    case '>':
      if (ch1 == '>')
        return SHR;
    default:
      return INVALID;
    }
  }

  bool IsFile( ) const { return file_; }
  std::string GetFileName( ) { return file_name_; }

  TokenType ThreeCharacterSymbol(char ch1, char ch2, char ch3)
  { // returns the type of symbol of three characters
    switch (ch3) {
    case '=':
      if (ch1 == '>' && ch2 == '>')
        return SHREQ;
      if (ch1 == '<' && ch2 == '<')
        return SHLEQ;
    default:
      return INVALID;
    }
  }

  void PutBack(Token *tok)
  { // put in the buffer for one token lookahead required by parser
    buffer_.push_back(tok);
  }

  std::string GetErrorReport( ) {
    return err_msg_;
  }
private:
  std::string code_;   // whole code will stored here
  int seek_;            // current position of the seek
  int save_end_;
  Position position_;   // current position in terms of line no and column no.
  int lastColNumber_;   // buffer required for GoBack()
  int end_;
  Token *tok_;          // buffer required for PutBack()
  std::size_t size_;    // size_ of the string code_
  std::string file_name_;
  std::string err_msg_;
  short status_;        // status of the lexer
  bool eos_;            // end of file flag
  bool file_;
  std::list<Token*> buffer_;
};

static void MakeLexer(Lexer **lex, std::string &str) {
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

#endif

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
//  \file:  token.h
//  \description: Definition of tokens and Token class used in grok
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <iostream>

enum TokenType {
  LPAR,
  RPAR,
  LBRACE,
  RBRACE,
  LSQB,
  RSQB,
  COLON,
  SCOLON,
  SCOMMA,
  INVCOMMA,
  DOT,
  COMMA,
  CONDITION,
  PLUS,
  MINUS,
  DIV,
  MUL,
  MOD,
  GT,
  LT,
  ASSIGN,
  XOR,
  BOR,
  BAND,
  NOT,
  BNOT,
  INC,
  DEC,
  PLUSEQ,
  MINUSEQ,
  DIVEQ,
  MODEQ,
  MULEQ,
  GTE,
  LTE,
  EQUAL,
  OR,
  AND,
  BOREQ,
  BANDEQ,
  XOREQ,
  SHL,
  SHR,
  NOTEQ,
  SHLEQ,
  SHREQ,
  LET,
  VAR,
  JSNULL,
  WITH,
  THEN,
  IN,
  FUNC,
  BREAK,
  FOR,
  IF,
  ELSE,
  DO,
  WHILE,
  CLASS,
  THIS,
  BOOL,
  TRUE,
  FALSE,
  NEW,
  DELETE,
  CASE,
  CONTINUE,
  RET,
  IDENT,
  INVALID,
  DIGIT,
  STRING,
  POUND,
  CMCOMM,
  EOS
};
//
static std::string token_type[] = {
    "LPAR",      "RPAR",     "LBRACE", "RBRACE",   "LSQB",    "RSQB",
    "COLON",     "SCOLON",   "SCOMMA", "INVCOMMA", "DOT",     "COMMA",
    "CONDITION", "PLUS",     "MINUS",  "DIV",      "MUL",     "MOD",
    "GT",        "LT",       "ASSIGN", "XOR",      "BOR",     "BAND",
    "NOT",       "BNOT",     "INC",    "DEC",      "PLUSEQ",  "MINUSEQ",
    "DIVEQ",     "MODEQ",    "MULEQ",  "GTE",      "LTE",     "EQUAL",
    "OR",        "AND",      "BOREQ",  "BANDEQ",   "XOREQ",   "SHL",
    "SHR",       "NOTEQ",    "SHLEQ",  "SHREQ",    "LET",     "VAR",
    "JSNULL",    "WITH",     "THEN",   "IN",       "FUNC",    "BREAK",
    "FOR",       "IF",       "ELSE",   "DO",       "WHILE",   "CLASS",
    "THIS",      "BOOL",     "TRUE",   "FALSE",    "NEW",     "DELETE",
    "CASE",      "CONTINUE", "RET",    "IDENT",    "INVALID", "DIGIT",
    "STRING",    "POUND",    "CMCOMM", "EOS"};

static const int TOKEN_NUM = 76;

class Position {
public:
  Position(const int x, const int y) : row_(y), col_(x) {}

  Position(const Position &pos) : row_(pos.row_), col_(pos.col_) {}

  Position() : row_(0), col_(0) {}

  Position &operator=(const Position &pos) {
    row_ = pos.row_;
    col_ = pos.col_;
    return *this;
  }

  ~Position() {}

  int row_;
  int col_;
};

// this class holds the functions for finding
// the information of the type of the operator
// Token class: this class will hold all the details
// of a particular token in this case, current token
class Token {
public:
  std::string value_;
  TokenType type_;
  int precedance_;
  Position position_;

  Token(std::string name, int type, int prec = -1)
      : type_{static_cast<TokenType>(type)}, precedance_{prec} {
    value_ = name;
  }

  Token(std::string name, int type, int prec, const Position &pos)
      : value_(name), type_(static_cast<TokenType>(type)), precedance_(prec),
        position_(pos) {}

  Token(const Token &tok)
      : value_(tok.value_), type_(tok.type_), precedance_(tok.precedance_),
        position_(tok.position_) {}

  Token &operator=(const Token &tok) { // copy from another token
    value_ = tok.value_;
    precedance_ = tok.precedance_;
    type_ = tok.type_;
    position_ = tok.position_;
    return *this;
  }

  Token() : value_{""}, type_{INVALID} {}

  ~Token() {}

  inline const std::string &GetValue() const { // return the value_ of the token
    return this->value_;
  }

  inline bool IsBinaryOperator() const {
    return (type_ >= PLUS && type_ <= LT) ||
           ((type_ >= XOR) && (type_ <= DEC)) ||
           ((type_ >= GTE) && (type_ <= AND)) ||
           ((type_ == SHL) && (type_ == SHR));
  }

  inline bool IsAssign() const {
    return (type_ == ASSIGN) || (type_ >= PLUSEQ && type_ <= MULEQ) ||
           (type_ >= BOREQ && type_ <= XOREQ) ||
           (type_ >= NOTEQ && type_ <= SHREQ);
  }

  inline TokenType type() const { return static_cast<TokenType>(type_); }

  void print(std::ostream &os, int tab = 0) {
    os << "Value: " << value_;
    os << "\nType: " << token_type[(int)type_];
    os << "\nPosition: " << position_.col_ << " | " << position_.row_;
  }
}; // Token

static Token TOKENS[TOKEN_NUM] = {
    // punctuations
    Token{"(", 0}, Token{")", 1}, Token{"{", 2}, Token{"}", 3},
    Token{"[", 4}, Token{"]", 5}, Token{":", 6}, Token{";", 7},
    Token{"'", 8}, Token{"\"", 9}, Token{".", 10}, Token{",", 11, 1},
    Token{"?", 12, 2},

    // operators
    Token{"+", 13, 12}, Token{"-", 14, 12}, Token{"/", 15, 13},
    Token{"*", 16, 13}, Token{"%", 17, 13}, Token{">", 18, 10},
    Token{"<", 19, 10}, Token{"=", 20, 2}, Token{"^", 21, 7}, Token{"|", 22, 6},
    Token{"&", 23, 8}, Token{"!", 24, 0}, Token{"~", 25, 0}, Token{"++", 26, 0},
    Token{"--", 27, 0}, Token{"+=", 28, 2}, Token{"-=", 29, 2},
    Token{"/=", 30, 2}, Token{"%=", 31, 2}, Token{"*=", 32, 2},
    Token{">=", 33, 10}, Token{"<=", 34, 10}, Token{"==", 35, 9},
    Token{"||", 36, 4}, Token{"&&", 37, 5}, Token{"|=", 38, 2},
    Token{"&=", 39, 2}, Token{"^=", 40, 2}, Token{"<<", 41, 11},
    Token{">>", 42, 11}, Token{"!=", 43, 9}, Token{"<<=", 44, 2},
    Token{">>=", 45, 2},

    // keywords
    Token{"let", LET}, Token{"var", VAR}, Token{"null", JSNULL},
    Token{"with", WITH}, Token{"then", THEN}, Token{"in", IN},
    Token{"function", FUNC}, Token{"break", BREAK}, Token{"for", FOR},
    Token{"if", IF}, Token{"else", ELSE}, Token{"do", DO},
    Token{"while", WHILE}, Token{"class", CLASS}, Token{"this", THIS},
    Token{"boolean", BOOL}, Token{"true", TRUE}, Token{"false", FALSE},
    Token{"new", NEW}, Token{"delete", DELETE}, Token{"case", CASE},
    Token{"continue", CONTINUE}, Token{"return", RET},

    // identfier
    Token{"", IDENT},

    // invalid token
    Token{"", INVALID},

    Token{"", DIGIT},

    // comments Token
    Token{"\"", STRING}, Token{"$", POUND}, Token{"*/", CMCOMM},

    // end of source
    Token{"EOS", EOS},
};

#endif

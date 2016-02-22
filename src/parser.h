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
//  \file:  parser.h
//  \description:  definition of Parser class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "astnode.h"
#include "jsfunction.h"
#include "jsobject.h"
#include "lexer.h"
#include "token.h"

#include <memory>

class Expression {
public:
  inline static std::string ParseIdentifier(Lexer *lexer);

  // deletes the current scope and heap associated with it
  void DeleteScope(int scope);

  // I used recursive-descent parser with one token lookahead so it is linear
  // in time complexity and requires no table generation.
  // some parser routines:
  static bool Primary(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Member(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Argument(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Constructor(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool ConstructorCall(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Unary(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Factor(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Term(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Shift(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Relational(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Equality(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool BitAnd(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool BitXor(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool BitOr(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool And(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Or(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Condition(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Assign(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Expr(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool ExprOpt(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Variable(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool Variables(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool VarOrExpr(Lexer *lexer, std::shared_ptr<AstNode> &expr);

  static bool Statement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool BlockStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool IfStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool WhileStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool ForStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool WithStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool ReturnStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr);
  static bool FunctionStatement(Lexer *lexer, std::shared_ptr<AstNode> &expr,
                                bool native,
                                NATIVE_HANDLE_TYPE native_handle = nullptr);

  static bool Parse(Lexer *lexer, std::shared_ptr<AstNode> &expr);

  static std::string err_msg_;
  static std::string file_name_;
};

#endif // EXPRESSION_H_

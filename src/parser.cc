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
//  \file:  parser.cc
//  \description:  implementation of parser class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#include "parser.h"

#include "jsarray.h"
#include "jsfunction.h"
#include "jsobject.h"
#include "lexer.h"
#include "jsbasicobject.h"
#include "object.h"
#include "token.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

// to store all the error messages
std::string Expression::err_msg_ = {};
std::string Expression::file_name_ = {};

// Expect function : Yeah it do the same thing that you are think
// Pass it a token pointer and type you want then it will return true
// if the token is that you are expecting otherwise will return false.
// Before returning false, it writes the message to a static variable
// err_msg_.
inline bool Expect(Token *tok, TokenType type) {
  if (tok->Type() != type) {
    Expression::err_msg_ =
        (std::string("SyntaxError:\n  ") + Expression::file_name_ + ":" +
         std::to_string(tok->position_.row_ + 1) + ":" +
         std::to_string(tok->position_.col_ + 1)) +
        ":" + " Expected " + TOKENS[type].GetValue();
    return false;
  }
  return true;
}

inline bool DoNotExpect(Token *tok,
                        TokenType type) { // Opposite of Expect Function
  if (tok->Type() == type) {
    Expression::err_msg_ =
        (std::string("Unexpected ") + TOKENS[type].value_ + " in line " +
         std::to_string(tok->position_.row_ + 1) + " at column " +
         std::to_string(tok->position_.col_ + 1));
    return true;
  }
  return false;
}

inline void Unexpected(Token *tok) {
  Expression::err_msg_ =
      (std::string("Unexpected ") + tok->GetValue() + " in line " +
       std::to_string(tok->position_.row_ + 1) + " at column " +
       std::to_string(tok->position_.col_ + 1));
}

std::string
Expression::ParseIdentifier(Lexer *lexer) { // parse the name of the identifier
  Token *tok = lexer->NextToken();
  std::string name = tok->GetValue();
  delete tok;
  return name;
}

// parse a primary expression
bool Expression::Primary(
    Lexer *lexer,
    std::shared_ptr<AstNode>
        &expr) { // parse a primary expression as given in the grammar
  Token *tok = lexer->NextToken();
  bool result = false;
  std::shared_ptr<JSNumber> number;
  std::string name;
  std::shared_ptr<JSString> str;
  switch (tok->Type()) {
  case LPAR:
    // we are to parse paranthesized expression
    //      '(' Expression ')'
    delete tok;
    result = Expr(lexer, expr);
    if (!result) // we cannot parse more
      return false;
    tok = lexer->NextToken();
    if (!Expect(tok, RPAR))
      return false;
    break;

  case IDENT:
    // we are to parse the identifier
    lexer->PutBack(tok);
    name = ParseIdentifier(lexer);
    expr = std::make_shared<AstNode>(); // make some room
    expr->variable_ = (name);
    expr->SetRelation(IDENT);
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->obj_ = (std::make_shared<JSBasicObject>());
    break;

  case DIGIT:
    // parse a number
    number = std::make_shared<JSNumber>(JSNumber(tok->GetValue()));
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(DIGIT);
    expr->obj_ = (number);
    break;

  case STRING:
    // parse a javascript string
    delete tok;
    str = std::make_shared<JSString>(JSString(lexer->GetStringLiteral()));
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(STRING);
    expr->obj_ = (str);
    break;

  case TRUE:
  case FALSE:
    // parse a js boolean
    // currently a boolean number is a JSNumber
    number =
        std::make_shared<JSNumber>(JSNumber((tok->Type() == TRUE) ? 1 : 0));
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(BOOL);
    expr->obj_ = (number);
    break;

  case JSNULL:
    // a null object
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(TokenType::JSNULL);
    expr->obj_ = std::make_shared<JSBasicObject>();
    break;

  case LSQB: {
    // parse a Javascript array which has the following form
    //     '[' primary ',' primary ',' ...... ']'
    // that we're parsing an array
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(TokenType::LSQB);
    expr->SetSecRelation(LSQB);
    delete tok;
    tok = lexer->NextToken();
    result = true;

    // continue this loop until we have found a right bracket
    while (tok->Type() != RSQB && result) {
      expr->AddChild(std::make_shared<AstNode>());
      lexer->PutBack(tok);
      result = Assign(lexer, expr->GetLastChild());
      if (result) {
        tok = lexer->NextToken();
        if (tok->Type() != COMMA && tok->Type() != RSQB) {
          Expect(tok, RSQB);
          break;
        } else if (tok->Type() == COMMA) {
          delete tok;
          tok = lexer->NextToken();
        }
      }
    }
    delete tok;
    return result;
  }

  case LBRACE: { // that we're parsing an object
    // A javascript object is a set of key : value pairs seperated by commas
    // Currently the parser for object is not complete, so beware before using
    // objects
    expr->SetSecRelation(LBRACE);
    expr->SetRelation(LBRACE);
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    delete tok;
    auto obj = std::make_shared<JSObject>();
    tok = lexer->NextToken();
    result = true;
    while (tok->Type() != RBRACE && result) {
      expr->AddChild(std::make_shared<AstNode>());
      result = Expect(tok, IDENT); // todo : make it valid for string also
      if (result) {
        expr->GetLastChild()->variable_ = tok->GetValue();
        delete tok;
        tok = lexer->NextToken();
        if (Expect(tok, COLON)) {
          delete tok;
          expr->GetLastChild()->AddChild(std::make_shared<AstNode>());
          if (Assign(lexer, expr->GetLastChild()->GetNthChild(0))) {
            tok = lexer->NextToken();
            if (tok->Type() != COMMA && tok->Type() != RBRACE) {
              Expect(tok, RBRACE);
              return false;
            } else if (tok->Type() == COMMA) {
              delete tok;
              tok = lexer->NextToken();
            }
          } else
            return false;
        } else {
          delete tok;
          return false;
        }
      } else {
        delete tok;
        return false;
      }
    }
    delete tok;
    return result;
  }

  case FUNC:
    // we're parsing a function
    lexer->PutBack(tok);
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    result = FunctionStatement(lexer, expr, false);
    return result;
    break;

  case THIS:
    // call to this object
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_primary;
    expr->SetRelation(TokenType::THIS);
    return true;
  // TODO:
  // unreachable

  default:
    // token that we don't recognise
    Unexpected(tok);
    lexer->PutBack(tok);
  }
  return true;
}

// Parse a member expression which can be primary or an object's member
// or array member or a function call
// member == >  primary
//              primary.member
//              primary[Expression]
//              primary(argument)
bool Expression::Member(
    Lexer *lexer, std::shared_ptr<AstNode> &
                      expr) { // parse member expression as described in grammar
  if (!Primary(lexer, expr)) {
    printf("Primary was expected!");
    return false;
  }

  bool result = true;
  std::shared_ptr<AstNode> node = std::make_shared<AstNode>();
  node->AddChild(expr);
  node->expression_type_ = AstNode::ExpressionType::_member;
  int i = 0;
  Token *tok = nullptr;

  do {
    tok = lexer->NextToken();
    switch (tok->Type()) {
    // primary.member
    case DOT:
      delete tok;
      i++;
      node->AddChild(std::make_shared<AstNode>());
      if (!Primary(lexer, node->GetLastChild())) {
        printf("Some error occurred");
      }
      node->GetLastChild()->SetSecRelation(DOT);
      break;

    // primary[expression]
    case LSQB:
      delete tok;
      node->AddChild(std::make_shared<AstNode>());
      i++;
      if (Expr(lexer, node->GetLastChild())) {
        tok = lexer->NextToken();
        result = Expect(tok, RSQB);
      }
      node->GetLastChild()->SetSecRelation(LSQB);

      break;

    // primary(argument)
    case LPAR:
      lexer->PutBack(tok);
      node->AddChild(std::make_shared<AstNode>());
      i++;
      if (!Argument(lexer, node->GetLastChild())) {
        printf("Some error Occurred");
      }
      node->GetLastChild()->SetSecRelation(LPAR);
      break;

    default:
      lexer->PutBack(tok);
      result = false;
      break;
    }
  } while (result);
  expr = (!i) ? node->links_[0] : node;
  return true;
}

// parse an argument expression
// argument ==> empty
//              assign, argument
bool Expression::Argument(
    Lexer *lexer,
    std::shared_ptr<AstNode> &expr) { // parse the argument expression
  Token *tok = lexer->NextToken();
  Expect(tok, LPAR);
  delete tok;
  expr->expression_type_ = AstNode::ExpressionType::_argument;
  std::shared_ptr<AstNode> child;
  bool result = false;
  tok = lexer->NextToken();
  while (tok->Type() != RPAR) {
    lexer->PutBack(tok);
    child = std::make_shared<AstNode>();
    result = Assign(lexer, child);
    if (result) {
      expr->AddChild(child);
      tok = lexer->NextToken();
      if (tok->Type() != COMMA && tok->Type() != RPAR) {
        Expect(tok, COMMA);
        delete tok;
        return false;
      } else if (tok->Type() == COMMA) {
        if (DoNotExpect(tok, RPAR)) {
          delete tok;
          return false;
        }
        tok = lexer->NextToken();
      }
    } else
      return false;
  }
  delete tok;
  return true;
}

// Parse a constructor expression
// constructor ==> this . constructorcall
//                 constructorcall
bool Expression::Constructor(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  Token *tok = lexer->NextToken(); // get the next token
  bool result = false;

  switch (tok->Type()) {
  case THIS:
    delete tok;
    expr->SetRelation(THIS);
    tok = lexer->NextToken();
    result = Expect(tok, DOT);
    delete tok;

    if (result) {
      expr->SetSecRelation(DOT);
      expr->expression_type_ = AstNode::ExpressionType::_constructor;
      expr->AddChild(std::make_shared<AstNode>());
      result = Member(lexer, expr->GetNthChild(0));
      if (result) {
        return true;
      } else
        return false;
    } else
      return false;

  default:
    lexer->PutBack(tok);
    result = ConstructorCall(lexer, expr);
    return result;
  }
  // unreachable
  return false;
}

// Parse a constructorcall expression
// constructorcall ==> identifier
//                     identifier ( argument )
//                     identifier . constructorcall
bool Expression::ConstructorCall(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  Token *tok = lexer->NextToken();
  bool result = Expect(tok, IDENT);
  std::string name;

  if (result) {
    name = ParseIdentifier(lexer);
    expr->variable_ = (name);
    tok = lexer->NextToken();

    switch (tok->Type()) {
    case DOT:
      delete tok;
      expr->AddChild(std::make_shared<AstNode>());
      expr->SetRelation(DOT);
      expr->expression_type_ = AstNode::ExpressionType::_constructorcall;
      return (Member(lexer, expr->GetNthChild(0)));

    case LPAR:
      lexer->PutBack(tok); // Argument requires this token
      expr->AddChild(std::make_shared<AstNode>());
      expr->SetRelation(LPAR);
      expr->expression_type_ = AstNode::ExpressionType::_constructorcall;
      return (Argument(lexer, expr->GetNthChild(0)));

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

// Parse an unary expression
// unary    ==> member
//              member ++
//              member --
//              + unary
//              - unary
//              ++ member
//              -- member
//              new constructor
//              delete member
bool Expression::Unary(
    Lexer *lexer,
    std::shared_ptr<AstNode> &expr) { // parses the unary expression
  Token *tok = lexer->NextToken();
  bool result = false;

  switch (tok->Type()) {
  case PLUS:
  case MINUS:
    // unary plus / minus
    expr->SetRelation(tok->Type());
    delete tok;
    expr->expression_type_ = AstNode::ExpressionType::_unary;
    expr->AddChild(std::make_shared<AstNode>());
    return Unary(lexer, expr->GetNthChild(0));

  case INC:
  case DEC:
    // pre-increment or pre-decrement operator
    expr->SetRelation(tok->Type());
    delete tok;
    expr->expression_type_ = AstNode::ExpressionType::_unary;
    expr->AddChild(std::make_shared<AstNode>());
    return Member(lexer, expr->GetNthChild(0));

  case NEW:
    // javascript new expression for e.g. new Array(4, 3);
    expr->SetRelation(NEW);
    delete tok;
    expr->expression_type_ = AstNode::ExpressionType::_unary;
    expr->AddChild(std::make_shared<AstNode>());
    return Constructor(lexer, expr->GetNthChild(0));

  case DELETE:
    expr->SetRelation(DELETE);
    delete tok;
    expr->expression_type_ = AstNode::ExpressionType::_unary;
    expr->AddChild(std::make_shared<AstNode>());
    return Member(lexer, expr->GetNthChild(0));

  default:
    lexer->PutBack(tok);
    result = Member(lexer, expr);

    if (result) {
      tok = lexer->NextToken();
      auto node = std::make_shared<AstNode>();
      switch (tok->Type()) {
      case INC:
      case DEC:
        node->AddChild(expr);
        node->expression_type_ = AstNode::ExpressionType::_unary;
        node->SetRelation(tok->Type());
        node->SetSecRelation(PLUS); // to distinguish between pre and post
        expr = node;
        delete tok;
        return true;

      default:
        lexer->PutBack(tok);
        return true;
      }
    }
  }

  return false;
}

// Parse a factor expression
// factor   ==> unary
//              unary * factor
//              unary / factor
//              unary % factor
bool Expression::Factor(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  bool result = Unary(lexer, expr);
  Token *tok = nullptr;

  if (result) {
    std::shared_ptr<AstNode> node = std::make_shared<AstNode>();
    tok = lexer->NextToken();

    switch (tok->Type()) {
    case MUL:
    case DIV:
    case MOD:
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->SetRelation(tok->Type());
      delete tok;
      node->expression_type_ = AstNode::ExpressionType::_factor;
      result = Factor(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

// term     ==> factor
//              factor + term
//              factor - term
bool Expression::Term(Lexer *lexer, std::shared_ptr<AstNode> &expr) {

  // get first operand
  bool result = Factor(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case PLUS:
    case MINUS:
      node = std::make_shared<AstNode>();
      node->AddChild(expr);
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_term;
      result = Term(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

// shift    ==> term
//              term >> shift
//              term << shift
bool Expression::Shift(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = Term(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;
    switch (tok->type_) {
    case SHR:
    case SHL:
      node = std::make_shared<AstNode>();
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->SetRelation(tok->Type());
      delete tok;
      node->expression_type_ = AstNode::ExpressionType::_shift;
      result = Shift(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

// relational ==> shift
//                shift > relational
//                shift < relational
//                shift >= relational
//                shift <= relational
bool Expression::Relational(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = Shift(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case LT:
    case GT:
    case LTE:
    case GTE:
      node = std::make_shared<AstNode>();
      node->AddChild(expr);
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_relational;
      result = Relational(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::Equality(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = Relational(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case EQUAL:
    case NOTEQ:
      node = std::make_shared<AstNode>();
      node->AddChild(expr);
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_equality;
      result = Equality(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::BitAnd(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = Equality(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case BAND:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_bitand;
      result = BitAnd(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::BitXor(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = BitAnd(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case XOR:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_bitxor;
      result = BitXor(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::BitOr(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = BitXor(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case BOR:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_bitor;
      result = BitOr(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::And(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = BitOr(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case AND:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_and;
      result = And(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::Or(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = And(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->type_) {
    case OR:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_or;
      result = Or(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::Condition(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  bool result = Or(lexer, expr);
  Token *tok = nullptr;

  if (result) {
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    if (tok->Type() == CONDITION) {
      node = std::make_shared<AstNode>();
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_conditional;
      result = Assign(lexer, node->GetNthChild(1));

      if (result) {
        tok = lexer->NextToken();
        Expect(tok, COLON);
        delete tok;
        node->AddChild(std::make_shared<AstNode>());
        result = Assign(lexer, node->GetNthChild(2));

        if (result) {
          expr = node;
          return true;
        }
        return false;
      } else
        return false;
    } else {
      lexer->PutBack(tok);
      return true;
    }
  }

  return false;
}

// assign    ==> condition
//               condition = assign
//               condition += assign
//               condition -= assign
//               condition *= assign
//               condition /= assign
//               condition %= assign
//               condition &= assign
//               condition |= assign
//               condition ^= assign
//               condition >>= assign
//               condition <<= assign
bool Expression::Assign(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  // get first operand
  bool result = Condition(lexer, expr);
  Token *tok = nullptr;

  if (result) { // we're good
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    switch (tok->Type()) {
    case ASSIGN:
    case PLUSEQ:
    case MINUSEQ:
    case MULEQ:
    case DIVEQ:
    case MODEQ:
    case BANDEQ:
    case BOREQ:
    case XOREQ:
    case SHREQ:
    case SHLEQ:
    case NOTEQ:
      node = std::make_shared<AstNode>();
      node->SetRelation(tok->Type());
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      node->expression_type_ = AstNode::ExpressionType::_assignment;
      result = Assign(lexer, node->GetNthChild(1));
      if (result) {
        expr = node;
        return true;
      }
      return false;

    default:
      lexer->PutBack(tok);
      return true;
    }
  }
  return false;
}

bool Expression::Expr(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  bool result = Assign(lexer, expr);
  Token *tok = nullptr;

  if (result) {
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;

    if (tok->Type() == COMMA) {
      node = std::make_shared<AstNode>();
      delete tok;
      node->AddChild(expr);
      node->AddChild(std::make_shared<AstNode>());
      result = Expr(lexer, node->GetNthChild(0));
      if (result) {
        expr = node;
        return true;
      }
      return false;
    } else {
      lexer->PutBack(tok);
      return true;
    }
  }

  return false;
}

bool Expression::ExprOpt(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  return (Expr(lexer, expr));
}

bool Expression::Variable(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  Token *tok = lexer->NextToken();
  std::string name;
  bool result = false;
  expr = std::make_shared<AstNode>();

  if (tok->Type() == IDENT) {
    lexer->PutBack(tok);
    name = ParseIdentifier(lexer);
    tok = lexer->NextToken();
    expr->variable_ = (name);
    expr->SetRelation(TokenType::VAR);
    expr->expression_type_ = AstNode::ExpressionType::_declaration;
    if (tok->Type() == ASSIGN) {
      delete tok;

      tok = lexer->NextToken();

      switch (tok->Type()) {
      case LSQB:
        lexer->PutBack(tok);
        expr->AddChild(std::make_shared<AstNode>());
        return Primary(lexer, expr->GetLastChild());

      case FUNC:
        // we're parsing a function
        lexer->PutBack(tok);
        expr->expression_type_ = AstNode::ExpressionType::_declaration;
        expr->AddChild(std::make_shared<AstNode>());
        result = FunctionStatement(lexer, expr->GetLastChild(), false);
        if (result) {
          expr->variable_ = (name);
        }
        return result;
        break;

      case LBRACE:
        lexer->PutBack(tok);
        expr->AddChild(std::make_shared<AstNode>());
        return Primary(lexer, expr->GetLastChild());

      default:
        lexer->PutBack(tok);
        expr->AddChild(std::make_shared<AstNode>());
        result = Assign(lexer, expr->GetNthChild(0));
        if (result) {
          return true;
        }
        return false;
      }
    } else {
      lexer->PutBack(tok);
      return true;
    }
  }
  lexer->PutBack(tok);
  return false;
}

bool Expression::Variables(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  Token *tok = nullptr;
  expr = std::make_shared<AstNode>();
  expr->AddChild(std::make_shared<AstNode>());
  expr->expression_type_ = AstNode::ExpressionType::_declarations;
  expr->SetRelation(TokenType::VAR);

  while (Variable(lexer, expr->GetLastChild())) {
    tok = lexer->NextToken();
    if (tok->Type() == COMMA) {
      delete tok;
      expr->AddChild(std::make_shared<AstNode>());
      continue;
    } else {
      lexer->PutBack(tok);
      return true;
    }
  }

  return false;
}

bool Expression::VarOrExpr(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  Token *tok = lexer->NextToken();
  bool result = false;
  std::shared_ptr<AstNode> node;

  switch (tok->Type()) {
  case VAR:
  case LET:
    node = std::make_shared<AstNode>();
    delete tok;
    node->expression_type_ = AstNode::ExpressionType::_declarations;
    result = Variables(lexer, node);
    if (result) {
      expr = node;
      return true;
    }
    return false;

  case EOS:
    return false;

  default:
    lexer->PutBack(tok);
    return Expr(lexer, expr);
  }
  return false;
}

// Statement:
//          ;
//          if Condition Statement
//          if Condition Statement else Statement
//          while Condition Statement
//          for ( ; ExpressionOpt ; ExpressionOpt ) Statement
//          for (VariablesOrExpression ; ExpressionOpt ; ExpressionOpt )
//          Statement
//          for ( VariablesOrExpression in Expression ) Statement
//          break ;
//          continue ;
//          with ( Expression ) Statement
//          return ExpressionOpt ;
//          CompoundStatement
//          VariablesOrExpression ;
bool Expression::Statement(
    Lexer *lexer,
    std::shared_ptr<AstNode> &expr) { // parse a javascript statement
  Token *tok = lexer->NextToken();
  bool result = false;

  switch (tok->Type()) {
  case SCOLON:
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_empty;
    return true;

  case IF:
    lexer->PutBack(tok);
    result = IfStatement(lexer, expr);
    return result;

  case WHILE:
    lexer->PutBack(tok);
    return WhileStatement(lexer, expr);

  case FOR:
    lexer->PutBack(tok);
    return ForStatement(lexer, expr);

  case BREAK:
    delete tok;
    tok = lexer->NextToken();
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_break;
    result = Expect(tok, SCOLON);
    delete tok;
    return result;

  case CONTINUE:
    delete tok;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_continue;
    tok = lexer->NextToken();

    // find a better way to ignore the semicolon mistakes
    result = Expect(tok, SCOLON);
    return result;

  case WITH:
    lexer->PutBack(tok);
    return WithStatement(lexer, expr);

  case RET:
    lexer->PutBack(tok);
    return ReturnStatement(lexer, expr);

  case LBRACE:
    lexer->PutBack(tok);
    return BlockStatement(lexer, expr);

  case FUNC:
    lexer->PutBack(tok);
    return FunctionStatement(lexer, expr, 0);

  default:
    lexer->PutBack(tok);
    result = VarOrExpr(lexer, expr);
    if (result) {
      tok = lexer->NextToken();
      result = Expect(tok, SCOLON);
      delete tok;
      return result;
    } else {
      return false;
    }
  }
  return false;
}

bool Expression::BlockStatement(
    Lexer *lexer,
    std::shared_ptr<AstNode> &expr) { // parse a block of statements
  Token *tok = lexer->NextToken();
  bool result = Expect(tok, LBRACE);
  delete tok;

  if (result) {
    tok = lexer->NextToken();
    std::shared_ptr<AstNode> node;
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_compound;

    // this loop will go on until a right brace or an error has occurred..
    // It adds the other statements as the child into the node as expr
    while (tok->Type() != RBRACE) {
      lexer->PutBack(tok); // putback the token that is of no concern here
      tok = nullptr;
      node = std::make_shared<AstNode>(); // fill the node with an empty pointer
      result = Statement(lexer, node);

      if (result) {
        expr->AddChild(node);
        tok = lexer->NextToken();
      } else
        break; // break from the loop as result is not good
    }
    if (tok)
      delete tok; // delete the RBRACE token
  }               // if (result)
  return result;
}

bool Expression::IfStatement(
    Lexer *lexer, std::shared_ptr<AstNode> &expr) { // parse a if statement
  Token *tok = lexer->NextToken();
  bool result = Expect(tok, IF); // here the token must be an if
  delete tok;
  tok = lexer->NextToken(); // get the next token
                            // now this token should be an open paranthesis '('
  if (!(result = Expect(tok, LPAR))) {
    printf("Expected an ( at <%d:%d>\n", tok->position_.col_,
           tok->position_.row_);
    delete tok;
    return false;
  }
  delete tok; // free the memory
  expr = std::make_shared<AstNode>();
  expr->AddChild(std::make_shared<AstNode>());
  expr->expression_type_ = AstNode::ExpressionType::_if;
  result = Expr(lexer, expr->GetNthChild(0));

  if (!result) {
    printf("Expected an expression in if statement\n");
    return false;
  }
  tok = lexer->NextToken();
  result = Expect(tok, RPAR); // expect a right paranthesis
  delete tok;
  if (!result) {
    printf("Expected a ) at <%d:%d>\n", tok->position_.row_,
           tok->position_.col_);
    return false;
  }
  expr->AddChild(std::make_shared<AstNode>());

  // parse the statement under the if condition, it should exist

  if (!Statement(lexer, expr->GetNthChild(1))) {
    printf("Expected a statement under if statement\n");
    return false;
  }

  tok = lexer->NextToken();
  if (tok->Type() == ELSE) {
    expr->AddChild(std::make_shared<AstNode>());
    delete tok;
    result = Statement(lexer, expr->GetNthChild(2));
    return result;
  } else {
    lexer->PutBack(tok);
    return true;
  }

  return false;
}

bool Expression::WhileStatement(
    Lexer *lexer, std::shared_ptr<AstNode> &expr) { // parse a while statement
  Token *tok = lexer->NextToken(); // get the next token from the lexer
  bool result = Expect(tok, WHILE);
  delete tok;

  tok = lexer->NextToken();
  // this token should be a left paranthesis
  if (!Expect(tok, LPAR)) {
    delete tok;
    return false;
  }
  delete tok; // free the memory
  expr = std::make_shared<AstNode>();
  expr->expression_type_ = AstNode::ExpressionType::_while;
  expr->AddChild(std::make_shared<AstNode>());

  // parse the condition or we can say the conditional expression
  if (!Expr(lexer, expr->GetNthChild(0))) {
    printf("Expected an expression in while (..)\n");
    return false;
  }
  tok = lexer->NextToken(); // get the next token
  if (Expect(tok, RPAR)) {
    delete tok;
    expr->AddChild(std::make_shared<AstNode>());

    // expect a statement here
    return Statement(lexer, expr->GetNthChild(1));
  } else {
    delete tok;
    return false;
  }

  return false;
}

bool Expression::ForStatement(
    Lexer *lexer, std::shared_ptr<AstNode> &expr) { // parse a for statement
  //      for (var i = foo ; i <= bar; i++)
  //      {
  //          // blah blah ...
  //      }
  //
  //  but in compiler language, they say this loop should be represented as
  //      for ( expression? ; expression ? ; expression? )
  //          statement
  //
  // the node of the for statement consists of three special nodes and last
  // node consists the statement that has to be looped. The 0th child of the
  // node consists initialization section, 1st node consists the condition
  // and the third node consists the update part of the loop, all three are
  // optional. The last node consists the main body of the for loop.

  Token *tok = lexer->NextToken();
  bool result = Expect(tok, FOR);
  delete tok;

  std::shared_ptr<AstNode> node = std::make_shared<AstNode>();
  node->expression_type_ = AstNode::ExpressionType::_for;
  tok = lexer->NextToken();
  if (!Expect(tok, LPAR))
    return false;

  VarOrExpr(lexer, expr);
  node->AddChild(expr);
  tok = lexer->NextToken();
  if (tok->type_ != SCOLON) {
    printf("Expected a ; at <%d:%d>\n", tok->position_.row_,
           tok->position_.col_);
    delete tok;
    return false;
  }
  delete tok;

  Expr(lexer, expr);
  node->AddChild(expr);

  tok = lexer->NextToken();
  if (tok->type_ != SCOLON) {
    printf("Expected a semicolon at <%d:%d>\n", tok->position_.row_,
           tok->position_.col_);
    delete tok;
    return false;
  }
  delete tok;

  Expr(lexer, expr);
  node->AddChild(expr);

  tok = lexer->NextToken();
  if (tok->type_ != RPAR) {
    printf("Expected a ) at <%d:%d>\n", tok->position_.row_,
           tok->position_.col_);
    delete tok;
    return false;
  }
  delete tok;

  expr = node;
  expr->AddChild(std::make_shared<AstNode>());
  return Statement(lexer, expr->GetLastChild());
}

bool Expression::WithStatement(
    Lexer *lexer,
    std::shared_ptr<AstNode>
        &expr) { // I don't know the actual working of with statement
  // So I regret but I didn't implemented it, so beware of using it
  return false;
}

bool Expression::ReturnStatement(
    Lexer *lexer, std::shared_ptr<AstNode> &expr) { // return statement
  // in terms of compiler language :
  // return Expression? ;
  Token *tok = lexer->NextToken();
  bool result = Expect(tok, RET);
  delete tok;

  if (result) {
    expr = std::make_shared<AstNode>();
    expr->expression_type_ = AstNode::ExpressionType::_return;
    expr->AddChild(std::make_shared<AstNode>());
    ExprOpt(lexer, expr->GetNthChild(0));
    tok = lexer->NextToken();
    result = Expect(tok, SCOLON);
    if (!result)
      lexer->PutBack(tok);
    else
      delete tok;
    return true;
  }
  return false;
}

bool Expression::FunctionStatement(
    Lexer *lexer, std::shared_ptr<AstNode> &expr, bool native,
    NATIVE_HANDLE_TYPE native_handle) { // function statement
  // Well, well, well, a little more...
  // This time we are parsing a function definition.
  // A javascript function is shown below,
  //  function Foo (a, b)
  //  {
  //       And yes ==> blah, blah...
  //  }
  //
  // In terms of Compiler notation, a normal javascript function has following
  // notation. ( uhmm, uhmm , a Little OFFICIAL touch )
  // __________________________________________________________________________
  //|                                                                          |
  //|           function Identifier? ( ParameterList? )                        |
  //|                  statement                                               |
  //|__________________________________________________________________________|
  //
  // So, following information will be stored in an AstNode of a function
  // definition
  //      1) Name of the function in the node itself,
  //      2) All the parameters and there names, as another nodes,
  //      3) And, a node consisting the statements under that function.
  //

  Token *tok = lexer->NextToken();
  bool result = Expect(tok, FUNC);
  delete tok;
  std::cout << "Function won't be parsed now." << std::endl;
  return false;
  // if (result) {

  //   std::shared_ptr<JSFunction> func;
  //   std::shared_ptr<AstNode> func_body = std::make_shared<AstNode>();
  //   // So, it is a function, it is the time to parse the name of the function.
  //   // But in Javascript the functions can be unnamed also so we have to parse
  //   // those function also.

  //   func = std::make_shared<JSFunction>(JSFunction());
  //   tok = lexer->NextToken();
  //   expr = std::make_shared<AstNode>();
  //   expr->expression_type_ = AstNode::ExpressionType::_function;

  //   // get the name of the function if it exists. Or in Compiler language IDENT?
  //   if (tok->Type() == IDENT) {
  //     expr->variable_ = (tok->GetValue());
  //     func->SetName(tok->GetValue());
  //     delete tok;
  //   }
  //   // else put back the token that we wrongly got in here
  //   else
  //     lexer->PutBack(tok);

  //   // now the left paranthesis exists
  //   tok = lexer->NextToken();
  //   result = Expect(tok, LPAR);
  //   delete tok;

  //   if (!result)
  //     return false;
  //   // get the parameters of the function. Or ICL ParameterList?
  //   std::vector<std::string> params;

  //   tok = lexer->NextToken();
  //   while (tok->Type() != TokenType::RPAR && result) {
  //     if (Expect(tok, TokenType::IDENT)) {
  //       params.push_back(tok->GetValue());
  //       delete tok; // free the memory
  //       tok = lexer->NextToken();

  //       // now next token should either be a RPAR or COMMA,
  //       // if it is neither of them then this is a token that we
  //       // don't expect here.
  //       if (tok->Type() != TokenType::COMMA && tok->Type() != TokenType::RPAR)
  //         result = false;
  //       else if (tok->Type() == TokenType::COMMA) {
  //         delete tok;
  //         tok = lexer->NextToken();
  //       }
  //     } else
  //       result = false;
  //   }
  //   func->SetParams(params);
  //   delete tok;
  //   if (native) {
  //     func->SetNativeHandle(native_handle);
  //     expr->expression_type_ = AstNode::ExpressionType::_native;
  //     expr->obj_ = func;
  //     return result;
  //   } else {
  //     // parsed all the parameters. Now, we have to parse the statements
  //     if (result) {
  //       result = BlockStatement(lexer, func_body);
  //       if (!result)
  //         return false;
  //       func->SetJSHandle(func_body);
  //       expr->obj_ = func;
  //     }
  //   }
  // }
  // return result;
}

bool Expression::Parse(Lexer *lexer, std::shared_ptr<AstNode> &expr) {
  file_name_ = lexer->GetFileName();
  expr = std::make_shared<AstNode>();
  Token *tok = lexer->NextToken();
  if (tok->Type() == EOS)
    return false;

  while (tok->Type() != TokenType::EOS) {
    lexer->PutBack(tok);
    expr->AddChild(std::make_shared<AstNode>());
    bool result = Statement(lexer, expr->GetLastChild());
    tok = lexer->NextToken();
    if (!result)
      return false;
  }
  return true;
}


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
//  \file:  interpreter.h
//  \description:  definition of interpreter class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "heap.h"
#include "jsarray.h"
#include "jsfunction.h"
#include "jsobject.h"
#include "object.h"
#include "parser.h"
#include "token.h"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <utility>

#define JS AstNode::ExpressionType

// print() javascript function. This function is called whenever print()
// is called within the script
static std::shared_ptr<AstNode> NativePrint(std::shared_ptr<AstNode>);

// Interpreter class : does simple interpretion of the javascript
// advanced features have not been implemented yet
class Interpreter {
  friend std::shared_ptr<AstNode> NativePrint(std::shared_ptr<AstNode>);
public:

  // static stack_ all the objects either from the external scripts or from
  // the shell are stored here. It visible everywhere using ::. So the native
  // functions can access those objects
  static js::Stack stack_;
  static js::Stack object_member_stack_;
  // default constructor
  Interpreter( ) :
    ptr_(nullptr), func_stack_( ), result_( ) {
    LoadNatives( );
  }

  Interpreter(std::shared_ptr<AstNode> &node) :
    ptr_(node)
  { }

  // loads the native function to the stack so that they can be called from
  // the script. All these function are global so that they can be accessed
  void LoadNatives( ) {
    LoadNative(std::string("function print(x)"), NativePrint);
  }

  // main interpreter function: call this function using a astnode generated
  // by parsing the script.
  std::shared_ptr<AstNode> ExecuteProgram(std::shared_ptr<AstNode> handle);

  // executes high level statements recursively
  std::shared_ptr<AstNode> ExecuteHighLevelStatement(std::shared_ptr<AstNode> node);

  // not implemented
  void PrintResult(std::ostream &os);
  void PrintAst(std::ostream &os);


  std::shared_ptr<AstNode> ExecuteLowLevelStatement(std::shared_ptr<AstNode> &node);
  std::shared_ptr<AstNode> BinaryOperator(std::shared_ptr<AstNode> &binary);

private:
  void PushObjectProperties(std::shared_ptr<JSBasicObject> obj);
  std::shared_ptr<AstNode> UnaryOperator(std::shared_ptr<AstNode> &unary);
  std::shared_ptr<AstNode> ArrayAccess(std::shared_ptr<AstNode> &node);
  std::shared_ptr<AstNode> MemberAccess(std::shared_ptr<AstNode> &node,
                                          bool can);
  std::shared_ptr<AstNode> MemberAccess(std::shared_ptr<AstNode>& node);
  std::shared_ptr<AstNode> FunctionCall(std::shared_ptr<AstNode> &node);
  std::shared_ptr<AstNode> Operate(std::shared_ptr<AstNode> &l,
                                    std::shared_ptr<AstNode> &r,
                                    TokenType operation);
  void DeclareVar(std::string &var, std::shared_ptr<AstNode> &data);
  std::shared_ptr<AstNode> AssignVar(std::string &var,
                                      std::shared_ptr<AstNode> &data);
  TokenType CheckType(std::shared_ptr<AstNode> &node);

  // copy from another node except the children
  std::shared_ptr<AstNode> CopyNode(std::shared_ptr<AstNode> rhs) {
    auto copy = std::make_shared<AstNode>( );
    copy->expression_type_ = rhs->expression_type_;
    copy->obj_ = rhs->obj_;
    copy->relation1_ = copy->relation1_;
    copy->variable_ = copy->variable_;
    return copy;
  }

  // load the native function by passing the prototype of the function as
  // a string and the native handle that must be called
  void LoadNative(std::string proto,
                  std::shared_ptr<AstNode>(*handle)(std::shared_ptr<AstNode>)) {
    Lexer lexer(proto);
    std::shared_ptr<AstNode> p;

    // parse the prototype of the function
    Expression::FunctionStatement(&lexer, p, true, handle);

    // although not required
    p->expression_type_ = JS::_native;
    std::pair<std::string, std::shared_ptr<AstNode>> info;
    info.first = p->variable_.GetName( );
    info.second = p;

    // push the variable onto stack
    stack_.PushVariable(info);
  }


  bool ConditionEvaluation(std::shared_ptr<AstNode> &node);

  std::shared_ptr<AstNode> ptr_;    // this holds the pointer to the expression
  std::stack<std::shared_ptr<AstNode>> func_stack_;
  std::stack<std::shared_ptr<AstNode>> ast_stack_;



  std::stack<std::shared_ptr<AstNode>> var_stack_;
  std::stack<TokenType> op_stack;
  std::shared_ptr<AstNode> result_;
};

// definition the NativePrint function
static std::shared_ptr<AstNode> NativePrint(std::shared_ptr<AstNode> node)
{
  // get the pointer to the parameter
  auto toprint = Interpreter::stack_.FindVariable(std::string("x"));
  if (toprint->obj_.get())
    std::printf("%s", toprint->obj_->ToString( ).c_str());
  else std::printf("undefined");
  std::printf("\n");
  return std::make_shared<AstNode>( );
}

#endif // INTERPRETER_H_

///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Prince Dhaliwal
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
//  \file:  astnode.h
//  \description:  definition of a single node of abstract syntax tree
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ASTNODE_H_
#define ASTNODE_H_

#include "jsobject.h"
#include "token.h"
#include <list>
#include <vector>
#include <iostream>

static std::string expression_type[] = {
  "undefined",    /* for undefined type */
  "_empty",
  "_primary",
  "_member",
  "_argument",
  "_array_access",
  "_constructor",
  "_constructorcall",
  "_declaration",
  "_declarations",
  "_unary",
  "_factor",
  "_functioncall",
  "_term",
  "_shift",
  "_relational",
  "_equality",
  "_bitand",
  "_bitxor",
  "_bitor",
  "_and",
  "_or",
  "_conditional",
  "_assignment",
  "_single",
  "_break",
  "_continue",
  "_compound",
  "if",
  "ifelse",
  "for",
  "for_in",
  "while",
  "_with",
  "_return",
  "_function",
  "_native"   /* tells whether the given node contains the native call */
};

// struct for Abstract Syntax Tree Node,
struct AstNode {
  // defines the type of the astnode
  // helpful in interpreting the node
  enum class ExpressionType {
    _undefined,    /* for undefined type */
    _empty,
    _primary,
    _member,
    _argument,
    _array_access,
    _constructor,
    _constructorcall,
    _declaration,
    _declarations,
    _unary,
    _factor,
    _functioncall,
    _term,
    _shift,
    _relational,
    _equality,
    _bitand,
    _bitxor,
    _bitor,
    _and,
    _or,
    _conditional,
    _assignment,
    _single,
    _break,
    _continue,
    _compound,
    _if,
    _ifelse,
    _for,
    _for_in,
    _while,
    _with,
    _return,
    _function,
    _native   /* tells whether the given node contains the native call */
  };

  // default constructor
  AstNode( ) :
    expression_type_(ExpressionType::_undefined),
    relation1_(TokenType::POUND), relation2_(POUND),
    obj_(nullptr), variable_(std::string("")),
    links_( )
  { }

  // create a astnode using the type of the expression
  AstNode(ExpressionType type) :
    expression_type_(type),
    relation1_(TokenType::POUND), relation2_(POUND),
    obj_(nullptr), variable_(std::string("")),
    links_( )
  { }

  // copy constructor
  AstNode(const AstNode &node) :
    expression_type_(node.expression_type_),
    relation1_(node.relation1_),
    relation2_(node.relation2_),
    obj_(node.obj_),
    variable_(node.variable_),
    links_(node.links_)
  { }

  // copy assignment
  AstNode &operator=(const AstNode &rhs) {
    expression_type_ = rhs.expression_type_;
    relation1_ = rhs.relation1_;
    relation2_ = rhs.relation2_;
    obj_ = rhs.obj_;
    variable_ = rhs.variable_;
    links_ = rhs.links_;
    return *this;
  }

  // Add a new child to the node. This is done by inserting a
  // blank node to the links_ vector in the end (using push_back)
  inline void AddChild(std::shared_ptr<AstNode> node)
                              { links_.push_back(node); }

  // set the primary relation i.e. relation1_
  inline void SetRelation(TokenType rel){ relation1_ = rel; }

  // removes the child from the node using vector::pop_back operation in links_
  inline void RemoveChild( ) { links_.pop_back( ); }

  // set the secondary relation, while secondary relation is not used
  // so it will be removed in the future
  inline void SetSecRelation(TokenType rel2){ relation2_ = rel2; }

  // get the nth child of the node, for now no index checking has been done
  // so, prior knowledge of the number of children is required
  inline std::shared_ptr<AstNode> &GetNthChild(int i){ return links_[i]; }

  // get the number of children
  inline size_t GetChildrenNumber( ) const { return links_.size( ); }

  // get the primary relation
  inline TokenType GetRelation( ) const { return relation1_; }

  // get the secondary relation, as SetSecRelation this function will also
  // be removed from the structure
  inline TokenType GetSecRelation( ) const { return relation2_; }

  // get the last child of the node, as the children are stored in the vector
  // or sequence, so it uses the vector::back() operation
  inline std::shared_ptr<AstNode>& GetLastChild( ){ return links_.back( ); }

  void print(std::ostream &os, int tab = 0) {
    std::string sp = "";
    sp.resize(3 * tab, ' ');
    os << sp << "Name: " << variable_.GetName();
    if (obj_.get()) {
      os <<  "\n" << sp << "JSBasicObject: ";
      obj_->print(os);
    }
    os << "\n" << sp << "ExpressionType: " << expression_type[(int)expression_type_];
    os << "\n" << sp << "Links: " << links_.size();
    os << "\n" << sp << "Relation1: " << token_type[(int)relation1_];
    os << "\n" << sp << "Relation2: " << token_type[(int)relation2_];
  }

  std::shared_ptr<JSBasicObject> obj_;    // Javascript object is stored here
  ExpressionType expression_type_;        // type of the expression
  std::vector<std::shared_ptr<AstNode>> links_;
                                          // children of the node
  JSVariable variable_;                   // useful for storing the name of
                                          // javascript variables

  // relation1_ is basically TokenType object. It defines the action of the
  // expression. For e.g. for a expression 2 + 3, the expression_type_ variable
  // will contain the ExpressionType::_term and relation1_ will contain the
  // TokenType::PLUS
  TokenType relation1_;

  // not so useful and will be removed
  TokenType relation2_;
};

static void PrintAST(std::shared_ptr<AstNode> node,
                     std::ostream &os, int tab = 0) {
  os << "\n";
  node->print(os, tab++);
  for (auto &child : node->links_) {
    PrintAST(child, os, tab);
    os << "\n";
  }
}


#endif // ASTNODE_H_

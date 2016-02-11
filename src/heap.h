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
//  \file:  heap.h
//  \description:  definition of stack class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HEAP_H_
#define HEAP_H_


#include "jsobject.h"
#include "lexer.h"
#include "parser.h"

#include <algorithm>
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace js {
using namespace std;

class Stack;
static void PrintHeap(const Stack &stack);

// 	**Stack class**
// This class holds all the javascript objects and functions.
// It uses the std::map for storage because the map
// provides the constant time access. It uses the string as a key which is
// simply the name of the javascript object or function. Currently only
// block scope are handled so for now there is no closure or object scope.
// As shared_ptr are used so the garbage handling is implicit and much
// accurate, though it is little slower.
class Stack {
  friend void PrintHeap(const Stack &stack);
public:

  // default constructor
  Stack() {
    CreateScope();
  }

  // create a block scope
  void CreateScope() {
    stack_.push_back(map<string, shared_ptr<AstNode>>());
  }

  // remove the block scope
  // Removing the scope will simply removes all the javascript objects and
  // deletes them automatically because the reference count of these shared_ptr
  // goes to 0 (maybe not if there is a global reference or a reference from
  // a object which lie in the outer scope to this object)
  void RemoveScope() {
    stack_.pop_back();
  }

  // get the javascript object or variable from its name
  shared_ptr<AstNode> FindVariable(const string &name) {
    auto size = stack_.size();
    for (int i = size - 1; i >= 0; i--) {
      auto find = _Find(stack_.at(i), name);
      if (find.get()) {
        return find;
      }
    }
    return shared_ptr<AstNode>();
  }

  // push a variable onto stack in the current scope
  void PushVariable(pair<string, shared_ptr<AstNode>> var) {
    stack_.back().insert(var);
  }

  // push a variable onto global scope
  void PushGlobal(pair<string, shared_ptr<AstNode>> var) {
    stack_.front().insert(var);
  }

  // returns true if the variable with name <name> already exists in any scope
  bool Exists(const string &name) {
    auto size = stack_.size();
    for (int i = size - 1; i >= 0; i--) {
      if (_Exists(stack_.at(i), name)) {
        return true;
      }
    }
    return false;
  }

  void RemoveAllScopes() {
    stack_.clear();
  }

  // returns the variable from the current scope. Returns empty pointer
  // if there is no variable in the current scope
  shared_ptr<AstNode> FindVariableInScope(const std::string &name) {
    return _Find(stack_.back(), name);
  }
private:

  // internal _Find method
  shared_ptr<AstNode> _Find(map<string, shared_ptr<AstNode>> &inst,
                            const std::string &name) {
    auto find = inst.find(name);
    if (find != inst.end()) {
      return find->second;
    }
    return std::shared_ptr<AstNode>();
  }

  // internal _Exists method
  bool _Exists(map<string, shared_ptr<AstNode>> &inst,
               const std::string &name) {
    auto find = inst.find(name);
    if (find != inst.end()) {
      return true;
    }
    return false;
  }

  // kind of heap to the javascript variables
  // it is simply a vector of map's with each map
  // corresponding to a different scopes
  vector<map<string, shared_ptr<AstNode>>> stack_;
};

static void PrintMap(const map<string, shared_ptr<AstNode>> &m) {
  for (const auto &i : m) {
    printf("%s : ", i.first.c_str());
    PrintASTObject(i.second);
    printf("\n");
  }
}

static void PrintHeap(const Stack &stack) {
  if (stack.stack_.empty()) {
    printf("Heap is empty\n");
    return;
  }
  for (const auto &i : stack.stack_) {
    PrintMap(i);
  }
}


}
#endif

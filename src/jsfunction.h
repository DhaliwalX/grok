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
//  \file:  jsfunction.h
//  \description:  definition of JSFunction class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef JS_FUNCTION_H_
#define JS_FUNCTION_H_

#include "astnode.h"
#include "jsobject.h"
#include "program.h"
#include "bytecode.h"

#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class Machine;

typedef bool (*native_handle_t)(Machine *);

#define NATIVE_HANDLE(x) std::function<bool(Machine *)> x
#define NATIVE_HANDLE_TYPE native_handle_t

std::shared_ptr<AstNode> Add(std::shared_ptr<AstNode>);

// // A Javascript function class
// // As all the javascript functions are objects therefore it derives
// // from JSBasicObject so that it can be easily referenced
// class JSFunction : public JSBasicObject {
// public:
//   // there are two types of functions :
//   //        1) Functions defined in the script
//   //        2) Functions which are defined natively
//   enum class JSFunctionType : bool { _native, _js };

//   // default constructor
//   JSFunction()
//       : function_(), parameters_(), name_(), type_(JSFunctionType::_js),
//         native_handle_(nullptr) {}

//   // constructor for normal Javascript functions
//   JSFunction(const std::string &name, std::shared_ptr<AstNode> function,
//              std::vector<std::string> parameters)
//       : function_(function), name_(name), native_handle_(nullptr),
//         parameters_(parameters), type_(JSFunctionType::_js) {}

//   JSFunction(const JSFunction &rhs)
//       : function_(rhs.function_), name_(rhs.name_),
//         parameters_(rhs.parameters_), type_(rhs.type_),
//         native_handle_(rhs.native_handle_) {}

//   JSFunction &operator=(const JSFunction &rhs) {
//     function_ = rhs.function_;
//     name_ = rhs.name_;
//     parameters_ = rhs.parameters_;
//     native_handle_ = rhs.native_handle_;
//     type_ = rhs.type_;
//     return *this;
//   }

//   // constructor for native javascript functions
//   JSFunction(const std::string &name, NATIVE_HANDLE_TYPE native_handle) {
//     name_ = name;
//     type_ = JSFunctionType::_native;
//     native_handle_ = native_handle;
//   }

//   // true if function id native
//   bool IsNative() const { return type_ == JSFunctionType::_native; }

//   // returns native handle of the callback
//   NATIVE_HANDLE_TYPE GetNativeFunction() { return native_handle_; }

//   // return the parameters in the form of vector<string>
//   const std::vector<std::string> &GetParameters() const { return parameters_; }

//   // returns the javascripyt function callback
//   std::shared_ptr<AstNode> GetJSHandle() { return function_; }

//   void SetNativeHandle(NATIVE_HANDLE_TYPE handle) {
//     native_handle_ = handle;
//     type_ = JSFunctionType::_native;
//   }

//   void SetJSHandle(std::shared_ptr<AstNode> func) {
//     function_ = func;
//     type_ = JSFunctionType::_js;
//   }

//   void SetName(const std::string &name) { name_ = name; }
//   void SetParams(std::vector<std::string> params) { parameters_ = params; }

//   std::string ToString() const override {
//     std::string buf;
//     buf += "[ function " + name_ + " ";
//     buf += IsNative() ? "NATIVE ]" : " FUNCTION ]";
//     return buf;
//   }

//   // returns the type of the object
//   ObjectType GetType() const { return ObjectType::_function; }

//   std::string GetName() const { return this->name_; }
//   BytecodeProgram<Object, Bytecode> *function_body_;

// private:
//   // If the function is defined within the javascript then the
//   // callback will be stored here
//   std::shared_ptr<AstNode> function_;

//   // All the parameters are stored in a vector of string
//   std::vector<std::string> parameters_;

//   // name of the function
//   std::string name_;

//   // A native function is a C++ function with a prototype
//   //    std::shared_ptr<AstNode> [function-name](std::shared_ptr<AstNode>)
//   NATIVE_HANDLE_TYPE native_handle_;
//   JSFunctionType type_;
// };

#endif // JS_FUNCTION_H_

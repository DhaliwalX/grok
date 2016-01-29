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
//  \file:  jsobject.h
//  \description:  definition of JSBasicObject and other related classes
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef JS_OBJECT_H_
#define JS_OBJECT_H_

#include "lexer.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// A javascript variable (name only)
class JSVariable {
public:
  JSVariable(const std::string &name) :
    name_(name) {
  }

  JSVariable() :
    name_("") {
  }

  JSVariable(const JSVariable &var) :
    name_(var.name_) {
  }

  JSVariable &operator=(const JSVariable &var) {
    name_ = var.name_;
    return *this;
  }

  std::string &GetName() { return name_; }
  std::string GetName() const { return name_; }

private:
  std::string name_;
};

static std::string __type[7] = {
  "null",
  "undefined",
  "number",
  "string",
  "object",
  "array",
  "function"
};

class JSArray;
class JSObject;

// JSBasicObject: super class for every javascript object
// and javascript functions
// JSBasicObject
//    JSNumber
//    JSString
//    JSObject
//    JSArray
//    JSFunction
class JSBasicObject {
public:

  // type of javascript objects
  enum class ObjectType {
    _null,
    _undefined,
    _number,
    _string,
    _object,
    _array,
    _function
  };


  // default constructor
  JSBasicObject() = default;

  JSBasicObject(const JSBasicObject &obj) = default;
  JSBasicObject& operator=(const JSBasicObject &rhs) = default;

  virtual std::string ToString() const { return "undefined"; }

  virtual void print(std::ostream &os) const {
    os << "Type: " << __type[static_cast<int>(GetType())];
  }

  virtual long long &GetNumber() {
    long long k = 0;
    return k;
  }

  virtual ObjectType GetType() const { // returns the type of the javascript object
    return ObjectType::_undefined;
  }

  template <ObjectType T>
  bool is() { return GetType() == T; }

};

// JSNumber class holding a javascript number
// currently it supports only int64_t not double's
class JSNumber : public JSBasicObject {
public:
  // derive some constructors
  using JSBasicObject::JSBasicObject;

  JSNumber(long long num) :
    number_(num) {
  }

  JSNumber(const std::string &str) { number_ = std::stoll(str); }

  JSNumber(const JSNumber &number)
    : number_(number.number_) {
  }

  JSNumber &operator=(const JSNumber &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSNumber() :
    number_(0) {
  }

  long long &GetNumber() override {
    return number_;
  }

  ~JSNumber() {
  }

  inline long long GetValue() const { return number_; }
  inline long long &GetValue() { return number_; }
  ObjectType GetType() const { return ObjectType::_number; }
  std::string ToString() const override { return std::to_string(number_); }

private:
  long long number_;
};

class JSString : public JSBasicObject {
public:
  JSString(const std::string &str) :
    js_string_(str) {
  }

  JSString() :
    js_string_("") {
  }

  ~JSString() {
  }

  inline std::string &GetString() { return js_string_; }
  inline std::string GetString() const { return js_string_; }
  ObjectType GetType() const { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }

private:
  std::string js_string_;
};

#endif

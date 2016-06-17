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

#include "object.h"
#include <memory>
#include "exceptions.h"
#include <string>
#include <unordered_map>
#include <vector>

// A javascript variable (name only)
class JSVariable {
public:
  JSVariable(const std::string &name) : name_(name) {}

  JSVariable() : name_("") {}

  JSVariable(const JSVariable &var) : name_(var.name_) {}

  JSVariable &operator=(const JSVariable &var) {
    name_ = var.name_;
    return *this;
  }

  std::string &GetName() { return name_; }
  std::string GetName() const { return name_; }

private:
  std::string name_;
};

static std::string __type[7] = {"null",   "undefined", "number",  "string",
                                "object", "array",     "function"};


#define DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type) \
    { throw Exception(  \
            std::string("operator '" #op "' is not defined for '" \
                #type "'")); }

#define DEFINE_OPERATOR(op, type)       \
    virtual Object operator op(Object ) \
        DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type)


class JSArray;
class JSObject;
class JSString;
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
  JSBasicObject &operator=(const JSBasicObject &rhs) = default;
  virtual ~JSBasicObject() = default;

  virtual std::string ToString() const { return "undefined"; }

  virtual void print(std::ostream &os) const {
    os << "Type: " << __type[static_cast<int>(GetType())];
  }

  virtual long long &GetNumber() {
    long long k = 0;
    return k;
  }

  virtual ObjectType
  GetType() const { // returns the type of the javascript object
    return ObjectType::_undefined;
  }

  template <ObjectType T> bool is() { return GetType() == T; }

  virtual void reset(std::shared_ptr<JSBasicObject> ptr) { return; }
};

class JSString : public JSBasicObject {
public:
  JSString(const std::string &str) : js_string_(str) {}

  JSString() : js_string_("") {}

  ~JSString() {}

  inline std::string &GetString() { return js_string_; }
  inline std::string GetString() const { return js_string_; }
  ObjectType GetType() const override { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }

  std::shared_ptr<JSBasicObject> At(int i) {
    return std::dynamic_pointer_cast<JSBasicObject, JSString>(
        std::make_shared<JSString>(std::string() + (js_string_[i])));
  }

private:
  std::string js_string_;
};

// JSNumber class holding a javascript number
// currently it supports only int64_t not double's
class JSNumber : public JSBasicObject {
public:
  // derive some constructors
  using JSBasicObject::JSBasicObject;

  JSNumber(long long num) : number_(num) {}

  JSNumber(const std::string &str) { number_ = std::stoll(str); }

  JSNumber(const JSNumber &number) : number_(number.number_) {}

  JSNumber &operator=(const JSNumber &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSNumber() : number_(0) {}

  long long &GetNumber() override { return number_; }

  ~JSNumber() {}

  inline long long GetValue() const { return number_; }
  inline long long &GetValue() { return number_; }
  ObjectType GetType() const override { return ObjectType::_number; }
  std::string ToString() const override { return std::to_string(number_); }

private:
  long long number_;
};

static inline bool undefined_operation(JSBasicObject::ObjectType type) {
  return type == JSBasicObject::ObjectType::_object
          || type == JSBasicObject::ObjectType::_array
          || type == JSBasicObject::ObjectType::_undefined;
}

static Object operator+(std::shared_ptr<JSNumber> l, Object r) 
{
  auto type = r.as<JSBasicObject>()->GetType();

  if (type == JSBasicObject::ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else if (type == JSBasicObject::ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
    return Object(result);
  } else {
    throw Exception("Code reached a place where it"
                          " was not supposed to be");
  }
}

#define OPERATOR_FOR_NUMBER(op) \
static Object operator op(std::shared_ptr<JSNumber> l, Object r) \
{ \
  auto type = r.as<JSBasicObject>()->GetType(); \
  \
  if (type == JSBasicObject::ObjectType::_number) { \
    auto rhs = r.as<JSNumber>();  \
    auto result = std::make_shared<JSNumber>(l->GetNumber() \
                      op rhs->GetNumber()); \
    return Object(result);  \
  } else { \
    throw Exception("We can't apply operator '" #op "' on string"); \
  } \
}

OPERATOR_FOR_NUMBER(-)
OPERATOR_FOR_NUMBER(*)
OPERATOR_FOR_NUMBER(/)
OPERATOR_FOR_NUMBER(%)

static Object operator +(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSBasicObject>()->GetType();

  if (type == JSBasicObject::ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
  } else if (type == JSBasicObject::ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw Exception("Code reached a place where it"
                          " was not supposed to be");
}

static Object operator+(Object l, Object r)
{
  auto ltype = l.as<JSBasicObject>()->GetType();
  auto rtype = r.as<JSBasicObject>()->GetType();

  if (undefined_operation(ltype) || undefined_operation(rtype)) {
    DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(+, Object);
  }

  switch (ltype) {
  case JSBasicObject::ObjectType::_string:
  {
    auto strobject = l.as<JSString>();
    return strobject + r;
  }

  case JSBasicObject::ObjectType::_number:
  {
    auto numobject = l.as<JSNumber>();
    return numobject + r;
  }

  case JSBasicObject::ObjectType::_object:
  case JSBasicObject::ObjectType::_array:
  case JSBasicObject::ObjectType::_undefined:
  default:
    throw Exception("unknown type caught in Object operator+");
  }
}

#define OTHER_OPERATOR(op) \
static Object operator op (Object l, Object r) \
{ \
  auto ltype = l.as<JSBasicObject>()->GetType(); \
  auto rtype = r.as<JSBasicObject>()->GetType(); \
  \
  if (ltype != JSBasicObject::ObjectType::_number \
      && rtype != JSBasicObject::ObjectType::_number) \
    throw Exception("fatal: can't apply operator '" #op "'"); \
  \
  auto numobject = l.as<JSNumber>(); \
  return numobject op r; \
}

OTHER_OPERATOR(-)
OTHER_OPERATOR(*)
OTHER_OPERATOR(/)
OTHER_OPERATOR(%)

#endif


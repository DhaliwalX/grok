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
//  \file:  jsarray.h
//  \description:  definition of JSArray class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef JSARRAY_H_
#define JSARRAY_H_

#include "object/object.h"
#include "object/jsobject.h"

#include <memory>
#include <vector>

namespace grok { namespace obj {

class JSArray : public JSBasicObject {
public:
  using ObjectPointer = std::shared_ptr<Object>;
  using size_type = std::vector<ObjectPointer>::size_type;

  JSArray(const std::vector<ObjectPointer> &elements) { elements_ = elements; }

  JSArray() {}

  JSArray(const JSArray &arr) { // create a object by a copy from other object
    elements_ = (arr.elements_);
  }

  JSArray &operator=(const JSArray &arr) { // assign an other array
    elements_ = arr.elements_;
    return (*this);
  }

  ObjectPointer operator[](int i) const { // no index checking
    return elements_[i];
  }

  ObjectPointer &operator[](int i) { // no index checking
    return elements_[i];
  }

  ObjectPointer &At(int i) { return elements_[i]; }

  ObjectPointer At(int i) const { // no index checking
    return elements_[i];
  }

  size_type Size() const { // returns the size of the vector
    return elements_.size();
  }

  ObjectType GetType() const override { // returns the type of the javascript object
    return ObjectType::_array;
  }

  bool Erase(size_type idx) { // erases the element
    if (idx < Size())
      elements_[idx];
    elements_[idx] = nullptr;
    return true;
  }

  bool Empty() const { // returns true if the array is empty
    return elements_.empty();
  }

  void Push(const ObjectPointer &obj) { // pushes the element to the last
    elements_.push_back(obj);
  }

  void Pop(const ObjectPointer &obj) { // pops the last element
    elements_.pop_back();
  }

  void Clear() { elements_.clear(); }

  void resize(size_t size) {
    elements_.resize(size, std::make_shared<AstNode>());
  }

  std::string ToString() const override {
    std::string buff = "[ ";
    for (const auto &element : elements_) {
      buff += element->as<JSBasicObject>()->ToString();
      buff += ", ";
    }
    if (elements_.size()) {
      buff.pop_back();
      buff.pop_back();
    }
    buff += " ]";
    return buff;
  }

private:
  std::vector<ObjectPointer> elements_;
};

} // obj
} // grok
#endif // JSARRAY_H_

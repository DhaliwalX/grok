#ifndef JSARRAY_H_
#define JSARRAY_H_

#include "object/object.h"
#include "object/jsobject.h"

#include <memory>
#include <vector>

namespace grok { namespace obj {

class JSArray : public JSObject {
public:
  using ObjectPointer = std::shared_ptr<Object>;
  using size_type = typename std::vector<ObjectPointer>::size_type;
  using iterator = typename std::vector<ObjectPointer>::iterator;
  using reverse_iterator 
        = typename std::vector<ObjectPointer>::reverse_iterator;

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

  void Resize(size_type sz)
  {
    // elements_.resize(sz, std::make_shared<UndefinedObject>());
  }

  void Push(const ObjectPointer &obj) { // pushes the element to the last
    elements_.push_back(obj);
  }

  void Pop(const ObjectPointer &obj) { // pops the last element
    elements_.pop_back();
  }

  void Clear() { elements_.clear(); }

  iterator begin()
  {
    return elements_.begin();
  }

  iterator end()
  {
    return elements_.end();
  }

  reverse_iterator rbegin()
  {
    return elements_.rbegin();
  }

  reverse_iterator rend()
  {
    return elements_.rend();
  }

  std::string ToString() const override {
    std::string buff = "[ ";
    for (const auto &element : elements_) {
      buff += element->as<JSObject>()->ToString();
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

extern std::shared_ptr<Object> CreateArray(size_t size);

} // obj
} // grok
#endif // JSARRAY_H_

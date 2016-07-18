#ifndef JSARRAY_H_
#define JSARRAY_H_

#include "object/object.h"
#include "object/jsobject.h"

#include <vector>

namespace grok { namespace obj {

class JSArray : public JSObject {
public:
  using HandlePointer = std::shared_ptr<Handle>;
  using size_type = std::vector<HandlePointer>::size_type;
  using iterator = std::vector<HandlePointer>::iterator;
  using reverse_iterator 
        = std::vector<HandlePointer>::reverse_iterator;

  JSArray(const std::vector<HandlePointer> &elements) { elements_ = elements; }

  JSArray() {}

  JSArray(const JSArray &arr) { // create a object by a copy from other object
    elements_ = (arr.elements_);
  }

  JSArray &operator=(const JSArray &arr) { // assign an other array
    elements_ = arr.elements_;
    return (*this);
  }

  HandlePointer operator[](int i) { // no index checking
    return elements_[i];
  }

  HandlePointer At(size_type i) {
    if (i >= Size()) {
      return JSObject::GetProperty(std::to_string(i));
    }
    return elements_[i];
  }

  HandlePointer At(const std::string &prop);

  size_type Size() const
  { // returns the size of the vector
    return elements_.size();
  }

  ObjectType GetType() const override
  { // returns the type of the javascript object
    return ObjectType::_array;
  }

  bool Erase(size_type idx) { // erases the element
    if (idx < Size())
      elements_[idx];
    elements_[idx] = CreateUndefinedObject();
    return true;
  }

  bool Empty() const { // returns true if the array is empty
    return elements_.empty();
  }

  void Resize(size_type sz)
  {
    elements_.resize(sz, CreateUndefinedObject());
  }

  void Push(const HandlePointer &obj) { // pushes the element to the last
    elements_.push_back(obj);
  }

  HandlePointer Pop() { // pops the last element
    auto ret = elements_.back();
    elements_.pop_back();
    return ret;
  }

  void Assign(size_type idx, const HandlePointer &obj)
  {
    if (idx > Size()) {
      this->AddProperty(std::to_string(idx), obj);
      return;
    }
    elements_[idx] = obj;
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

  std::string ToString() const override;

  std::string AsString() const override;

  JSObject::Value GetProperty(const JSObject::Name &name) override;

  auto &Container() { return elements_; }
private:
  std::vector<HandlePointer> elements_;

public:
  static std::shared_ptr<Handle> array_handle;
  static void Init();
  static std::pair<std::shared_ptr<Handle>, bool>
   GetStaticProperty(const std::string &str);
};

static inline bool IsJSArray(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_array;
}

extern std::shared_ptr<Object> CreateArray(size_t size);

} // obj
} // grok
#endif // JSARRAY_H_

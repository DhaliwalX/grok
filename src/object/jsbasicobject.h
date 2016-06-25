#ifndef OBJECT_H_
#define OBJECT_H_

#include "object/object.h"
#include "object/jsobject.h"

#include <unordered_map>

namespace grok { namespace obj {

// A javascript object is a set of name : value pair
class JSObject : public JSBasicObject {
public:
  using Name = std::string;
  using Value = std::shared_ptr<Object>;
  using iterator = std::unordered_map<Name, Value>::iterator;
  using const_iterator = std::unordered_map<Name, Value>::const_iterator;

  JSObject(const std::unordered_map<Name, Value> &map) : object_(map) {}

  JSObject() : object_() {}

  JSObject(const JSObject &obj) : object_(obj.object_) {}

  JSObject &operator=(const JSObject &obj) { // assigned from other object
    object_ = obj.object_;
    return (*this);
  }

  ~JSObject() { object_.clear(); }

  ObjectType GetType() const override { // returns the type of the javascript object
    return ObjectType::_object;
  }

  // add a new property to the object
  void AddProperty(const Name &name, const Value &prop) {
    object_.insert({name, prop});
  }

  // remove a property currently existing in the object
  void RemoveProperty(const Name &name) { object_.erase(name); }

  // returns true if a property exists in the object
  bool HasOwnProperty(const Name &name) {
    return object_.find(name) != object_.end();
  }

  Value GetProperty(const Name &name) {
    auto value = object_.find(name);
    if ((value) != object_.end()) {
      return value->second;
    }
    return Value();
  }

  iterator begin() { return object_.begin(); }

  iterator end() { return object_.end(); }

  void Clear() { object_.clear(); }

  std::string ToString() const override {
    std::string buff = "";
    buff += "{ ";
    for (const auto &it : object_) {
      buff += it.first + ": ";
      buff += it.second->as<JSBasicObject>()->ToString() += ", ";
    }
    buff.pop_back(); // ","
    buff.pop_back(); // " "
    buff += " }";
    return buff;
  }

private:
  std::unordered_map<Name, Value> object_;
};

} // obj
} // grok
#endif // OBJECT_H_

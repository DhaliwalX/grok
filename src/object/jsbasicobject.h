#ifndef OBJECT_H_
#define OBJECT_H_

#include "object/object.h"

#include <unordered_map>

namespace grok { namespace obj {

// type of javascript objects
enum class ObjectType {
  _null,
  _undefined,
  _number,
  _string,
  _object,
  _array,
  _argument,
  _function
};

static inline std::shared_ptr<Object> CreateUndefinedObject();

// A javascript object is a set of name : value pair
class JSObject : public std::enable_shared_from_this<JSObject> {
public:
  using Name = std::string;
  using Value = std::shared_ptr<Object>;
  using iterator = std::unordered_map<Name, Value>::iterator;
  using const_iterator = std::unordered_map<Name, Value>::const_iterator;

  JSObject(const std::unordered_map<Name, Value> &map) : object_(map) {}

  JSObject() : object_(), enumerable_{ true }, writable_{ true } {}

  JSObject(const JSObject &obj) : object_(obj.object_), writable_{ true } {}

  virtual ~JSObject() { }

  virtual ObjectType GetType() const
  {
  // returns the type of the javascript object
    return ObjectType::_object;
  }

  // add a new property to the object
  void AddProperty(const Name &name, const Value &prop) {
    object_.insert({name, prop});
  }

  // remove a property currently existing in the object
  void RemoveProperty(const Name &name) { object_.erase(name); }

  // returns true if a property exists in the object
  bool HasProperty(const Name &name) {
    return object_.find(name) != object_.end();
  }

  virtual Value GetProperty(const Name &name) {
    auto value = object_.find(name);
    if ((value) != object_.end()) {
      return value->second;
    }
    auto def = CreateUndefinedObject();
    AddProperty(name, def);
    return def;
  }

  bool IsEnumerable() const { return enumerable_; }

  void SetNonEnumerable() { enumerable_ = false; }

  bool IsWritable() const { return writable_; }

  void SetNonWritable() { writable_ = false; }

  iterator begin() { return object_.begin(); }

  iterator end() { return object_.end(); }

  void Clear() { object_.clear(); }

  virtual std::string ToString() const;
  virtual std::string AsString() const;

  virtual bool IsTrue() const
  {
    return true;
  }

private:
  std::unordered_map<Name, Value> object_;
  bool enumerable_;
  bool writable_;
};

class JSNull : public JSObject {
  std::string ToString() const override
  {
    return "null";
  }

  ObjectType GetType() const override
  {
    return ObjectType::_null;
  }
};

class UndefinedObject : public JSObject {
public:
  UndefinedObject() :
    JSObject{}
  {
  }

  std::string ToString() const override
  {
    return "undefined";
  }

  ObjectType GetType() const override
  {
    return ObjectType::_undefined;
  }

  bool IsTrue() const override
  {
    return false;
  }
};

static inline bool CanEvaluateToTrue(std::shared_ptr<Object> obj)
{
  auto type = obj->as<JSObject>()->GetType();

  return (type != ObjectType::_undefined)
          || (type != ObjectType::_null);
}

static std::shared_ptr<Object> CreateUndefinedObject()
{
  auto Undef = std::make_shared<UndefinedObject>();
  auto UndefWrapper = std::make_shared<Object>(Undef);
  return UndefWrapper;
}

static inline bool IsUndefined(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_undefined;
}

static inline bool IsNull(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_null;
}

extern void DefineInternalObjectProperties(JSObject *obj);

extern std::shared_ptr<Object> CreateJSObject();

extern std::shared_ptr<Object> CreateCopy(std::shared_ptr<Object> obj);
} // obj
} // grok
#endif // OBJECT_H_

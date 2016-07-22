#ifndef OBJECT_H_
#define OBJECT_H_

#include "object/object.h"

#include <map>

namespace grok { namespace obj {

// type of javascript objects
enum class ObjectType {
  _null,
  _undefined,
  _bool,
  _number,
  _double,
  _string,
  _object,
  _array,
  _argument,
  _function
};


template <class Key, class Value>
using PropertyContainer = std::map<Key, Value>;
static inline std::shared_ptr<Handle> CreateUndefinedObject();

// A javascript object is a set of name : value pair
class JSObject {
public:
  using Name = std::string;
  using Value = std::shared_ptr<Handle>;
  using iterator = PropertyContainer<Name, Value>::iterator;
  using const_iterator = PropertyContainer<Name, Value>::const_iterator;

  JSObject(const PropertyContainer<Name, Value> &map) : object_(map) {}

  JSObject() : object_(), enumerable_{ true }, writable_{ true } {}

  JSObject(const JSObject &obj) : object_(obj.object_), writable_{ true } {}

  virtual ~JSObject() { }

  virtual ObjectType GetType() const
  {
  // returns the type of the javascript object
    return ObjectType::_object;
  }

  // add a new property to the object
  virtual void AddProperty(const Name &name, const Value &prop) {
    object_[name] = prop;
  }

  // remove a property currently existing in the object
  virtual void RemoveProperty(const Name &name) { object_.erase(name); }

  // returns true if a property exists in the object
  virtual bool HasProperty(const Name &name) {
    return object_.find(name) != object_.end();
  }

  virtual Value GetProperty(const Name &name);

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
  PropertyContainer<Name, Value> object_;
  bool enumerable_;
  bool writable_;

public:
  static std::shared_ptr<Handle> st_obj;
  static void Init();
  static std::pair<std::shared_ptr<Handle>, bool>
    GetStaticProperty(const std::string &name);
};

class JSNull : public JSObject {
  std::string ToString() const override
  {
    return "null";
  }

  std::string AsString() const override
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

  std::string AsString() const override
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

static inline bool CanEvaluateToTrue(std::shared_ptr<Handle> obj)
{
  auto type = obj->as<JSObject>()->GetType();

  return (type != ObjectType::_undefined)
          || (type != ObjectType::_null);
}

static inline std::shared_ptr<Handle> CreateUndefinedObject()
{
  auto Undef = std::make_shared<UndefinedObject>();
  auto UndefWrapper = std::make_shared<Handle>(Undef);
  return UndefWrapper;
}

static inline std::shared_ptr<Handle> CreateJSNull()
{
  auto Null = std::make_shared<JSNull>();
  auto UndefWrapper = std::make_shared<Handle>(Null);
  return UndefWrapper;
}

static inline bool IsUndefined(std::shared_ptr<Handle> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_undefined;
}

static inline bool IsNull(std::shared_ptr<Handle> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_null;
}

extern void DefineInternalObjectProperties(JSObject *obj);


/// This is one of 5 other Create**() function. Each of these
/// function wraps the actual object in the Handle. So, Handle
/// is actually a reference to the object. That is in simple words,
/// these functions return a reference to the newly created object.
/// Name Handle is ambiguous and it should be Reference or Wrapper.
extern std::shared_ptr<Handle> CreateJSObject();

extern std::shared_ptr<Handle> CreateCopy(std::shared_ptr<Handle> obj);
} // obj
} // grok
#endif // OBJECT_H_

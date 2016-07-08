#ifndef JSNUMBER_H_
#define JSNUMBER_H_

#include "object/jsbasicobject.h"

namespace grok {
namespace obj {

extern std::shared_ptr<Object> CreateJSNumber(double num);
extern std::shared_ptr<Object> CreateJSNumber(std::string str);

// JSNumber class holding a javascript number
class JSNumber : public JSObject {
public:
  // derive some constructors
  using JSObject::JSObject;

  JSNumber(int32_t num) : number_(num) {}

  JSNumber(const std::string &str) { number_ = std::stoi(str); }

  JSNumber(const JSNumber &number) : number_(number.number_) {}

  JSNumber &operator=(const JSNumber &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSNumber() : number_(0) {}

  int32_t &GetNumber() { return number_; }

  ~JSNumber() {}

  inline int32_t GetValue() const { return number_; }
  inline int32_t &GetValue() { return number_; }
  ObjectType GetType() const override { return ObjectType::_number; }
  std::string ToString() const override { return std::to_string(number_); }
  std::string AsString() const override { return std::to_string(number_); }

  bool IsTrue() const override
  {
    return number_;
  }
private:
  int32_t number_;
};

// JSDouble class holding a javascript number
class JSDouble : public JSObject {
public:
  // derive some constructors
  using JSObject::JSObject;

  JSDouble(double num) : number_(num) {}

  JSDouble(const std::string &str) { number_ = std::stod(str); }

  JSDouble(const JSDouble &number) : number_(number.number_) {}

  JSDouble &operator=(const JSDouble &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSDouble() : number_(0) {}

  double &GetNumber() { return number_; }

  ~JSDouble() {}

  inline double GetValue() const { return number_; }
  inline double &GetValue() { return number_; }
  ObjectType GetType() const override { return ObjectType::_double; }
  std::string ToString() const override;
  std::string AsString() const override;

  bool IsTrue() const override
  {
    return number_;
  }
private:
  double number_;
};

static inline bool IsJSNumber(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_number
            || O->GetType() == ObjectType::_double;
}

static inline std::shared_ptr<Object>
 TryForNumber(std::shared_ptr<Object> obj)
{
  if (IsJSNumber(obj))
    return obj;
  auto str = obj->as<JSObject>()->ToString();
  auto maybenum = CreateJSNumber(str);

  if (IsUndefined(maybenum)) {
    return CreateJSNumber(0);
  } else {
    return maybenum;
  }
}

}
}

#endif

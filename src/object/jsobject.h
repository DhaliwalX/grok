#ifndef JS_OBJECT_H_
#define JS_OBJECT_H_

#include "object/object.h"
#include "common/exceptions.h"
#include "object/jsbasicobject.h"

#include <memory>
#include <string>

namespace grok { namespace obj {
extern std::string __type[7];

#define DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type) \
    { throw std::runtime_error(  \
            std::string("operator '" #op "' is not defined for '" \
                #type "'")); }

#define DEFINE_OPERATOR(op, type)       \
    virtual Object operator op(Object ) \
        DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type)

static inline std::shared_ptr<Object> CreateJSString(std::string str = "");

class JSString : public JSObject {
public:
  JSString(const std::string &str) : js_string_(str) {}

  JSString() : js_string_("") {}

  ~JSString() {}

  inline std::string &GetString() { return js_string_; }
  inline std::string GetString() const { return js_string_; }
  ObjectType GetType() const override { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }

  JSObject::Value GetProperty(const std::string &prop) override
  {
      int idx = 0;
      try {
          idx = std::stod(prop);
      } catch (...) {
          return JSObject::GetProperty(prop);
      }

      return CreateJSString(std::string() + js_string_[idx]);
  }

  bool IsTrue() const override
  {
    return js_string_.size();
  }

private:
  std::string js_string_;
};

// JSNumber class holding a javascript number
// currently it supports only int64_t not double's
class JSNumber : public JSObject {
public:
  // derive some constructors
  using JSObject::JSObject;

  JSNumber(long long num) : number_(num) {}

  JSNumber(const std::string &str) { number_ = std::stoll(str); }

  JSNumber(const JSNumber &number) : number_(number.number_) {}

  JSNumber &operator=(const JSNumber &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSNumber() : number_(0) {}

  long long &GetNumber() { return number_; }

  ~JSNumber() {}

  inline long long GetValue() const { return number_; }
  inline long long &GetValue() { return number_; }
  ObjectType GetType() const override { return ObjectType::_number; }
  std::string ToString() const override { return std::to_string(number_); }

  bool IsTrue() const override
  {
    return number_;
  }
private:
  long long number_;
};

static inline bool IsJSNumber(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_number;
}

static inline bool IsJSString(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_string;
}

static std::shared_ptr<Object> CreateJSString(std::string str)
{
    auto S = std::make_shared<JSString>(str);
    auto W = std::make_shared<Object>(S);
    return W;
}

extern Object operator+(std::shared_ptr<JSNumber> l, Object r);

#define DECLARE_OPERATOR(op)  \
extern Object operator op(std::shared_ptr<JSNumber> l, Object r)
DECLARE_OPERATOR(-);
DECLARE_OPERATOR(*);
DECLARE_OPERATOR(/);
DECLARE_OPERATOR(%);
#undef DECLARE_OPERATOR

extern Object operator +(std::shared_ptr<JSString> l, Object r);
extern Object operator+(Object l, Object r);

#define DECLARE_OTHER_OPERATOR(op) \
extern Object operator op (Object l, Object r)

DECLARE_OTHER_OPERATOR(*);
DECLARE_OTHER_OPERATOR(-);
DECLARE_OTHER_OPERATOR(/);
DECLARE_OTHER_OPERATOR(%);
#undef DECLARE_OTHER_OPERATOR

}
}
#endif


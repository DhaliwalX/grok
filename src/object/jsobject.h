#ifndef JS_OBJECT_H_
#define JS_OBJECT_H_

#include "object/object.h"
#include "common/exceptions.h"
#include "object/jsbasicobject.h"
#include "object/jsnumber.h"

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

extern std::shared_ptr<Object> CreateJSString(std::string str = "");

class JSString : public JSObject {
public:
  using size_type = std::string::size_type;

  JSString(const std::string &str) : js_string_(str) {}

  JSString() : js_string_("") {}

  ~JSString() {}

  inline std::string &GetString() { return js_string_; }
  inline std::string GetString() const { return js_string_; }
  ObjectType GetType() const override { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }
  
  std::string AsString() const override
  {
    return std::string("'") + js_string_ + "'";
  }
  
  JSObject::Value GetProperty(const std::string &prop) override
  {
      int idx = 0;
      try {
          idx = std::stod(prop);
      } catch (...) {
          if (prop == "length") {
              return CreateJSNumber(js_string_.size());
          }
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

static inline bool IsJSString(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_string;
}

#define DECLARE_OTHER_OPERATOR(op) \
extern Object operator op (Object l, Object r)

DECLARE_OTHER_OPERATOR(+);
DECLARE_OTHER_OPERATOR(*);
DECLARE_OTHER_OPERATOR(-);
DECLARE_OTHER_OPERATOR(/);
DECLARE_OTHER_OPERATOR(%);
DECLARE_OTHER_OPERATOR(==);
DECLARE_OTHER_OPERATOR(!=);
DECLARE_OTHER_OPERATOR(<);
DECLARE_OTHER_OPERATOR(>);
DECLARE_OTHER_OPERATOR(<=);
DECLARE_OTHER_OPERATOR(>=);
DECLARE_OTHER_OPERATOR(>>);
DECLARE_OTHER_OPERATOR(<<);
DECLARE_OTHER_OPERATOR(&);
DECLARE_OTHER_OPERATOR(|);
DECLARE_OTHER_OPERATOR(||);
DECLARE_OTHER_OPERATOR(&&);
DECLARE_OTHER_OPERATOR(^);
#undef DECLARE_OTHER_OPERATOR

}
}
#endif


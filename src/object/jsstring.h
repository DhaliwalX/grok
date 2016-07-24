#ifndef JSSTRING_H_
#define JSSTRING_H_

#include "object/jsbasicobject.h"

namespace grok {
namespace obj {

extern std::shared_ptr<Object> CreateJSString(std::string str = "");

class JSString : public JSObject {
public:
  using size_type = std::string::size_type;

  JSString(std::string str) : js_string_(std::move(str)) {}

  JSString() : js_string_("") {}

  ~JSString() {}

  inline std::string &GetString() { return js_string_; }
  ObjectType GetType() const override { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }
  
  std::string AsString() const override;
  
  JSObject::Value GetProperty(const std::string &prop) override;

  bool IsTrue() const override
  {
    return js_string_.size();
  }

  std::string& str() { return js_string_; }

  void concat(const std::string &str);

  static std::pair<std::shared_ptr<Handle>, bool>
    GetStaticProperty(const std::string&);

  static void Init();
private:
  std::string js_string_;

public:
  // this variable holds all string properties
  static JSString string;
};

static inline bool IsJSString(std::shared_ptr<Object> obj)
{
  auto O = obj->as<JSObject>();
  return O->GetType() == ObjectType::_string;
}


}
}

#endif

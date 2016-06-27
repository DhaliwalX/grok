#include "object/object.h"
#include "object/jsobject.h"


namespace grok { namespace obj {
std::string __type[7] = {"null",   "undefined", "number",  "string",
                                "object", "array",     "function"};

inline bool undefined_operation(ObjectType type) {
  return type == ObjectType::_object
          || type == ObjectType::_array
          || type == ObjectType::_undefined
          || type == ObjectType::_null;
}

Object operator+(std::shared_ptr<JSNumber> l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
    return Object(result);
  } else {
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
  }
}

#define OPERATOR_FOR_NUMBER(op) \
Object operator op(std::shared_ptr<JSNumber> l, Object r) \
{ \
  auto type = r.as<JSObject>()->GetType(); \
  \
  if (type == ObjectType::_number) { \
    auto rhs = r.as<JSNumber>();  \
    auto result = std::make_shared<JSNumber>(l->GetNumber() \
                      op rhs->GetNumber()); \
    return Object(result);  \
  } else { \
    throw std::runtime_error("We can't apply operator '" #op "' on string"); \
  } \
}

OPERATOR_FOR_NUMBER(-)
OPERATOR_FOR_NUMBER(*)
OPERATOR_FOR_NUMBER(/)
OPERATOR_FOR_NUMBER(%)
OPERATOR_FOR_NUMBER(==)
OPERATOR_FOR_NUMBER(!=)
OPERATOR_FOR_NUMBER(<)
OPERATOR_FOR_NUMBER(>)
OPERATOR_FOR_NUMBER(>>)
OPERATOR_FOR_NUMBER(<<)

Object operator +(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
    return Object(result);
  } else if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

Object operator+(Object l, Object r)
{
  auto ltype = l.as<JSObject>()->GetType();
  auto rtype = r.as<JSObject>()->GetType();

  if (undefined_operation(ltype) || undefined_operation(rtype)) {
    DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(+, Object);
  }

  switch (ltype) {
  case ObjectType::_string:
  {
    auto strobject = l.as<JSString>();
    return strobject + r;
  }

  case ObjectType::_number:
  {
    auto numobject = l.as<JSNumber>();
    return numobject + r;
  }

  case ObjectType::_object:
  case ObjectType::_array:
  case ObjectType::_undefined:
  default:
    throw std::runtime_error("unknown type caught in Object operator+");
  }
}

#define OTHER_OPERATOR(op) \
Object operator op (Object l, Object r) \
{ \
  auto ltype = l.as<JSObject>()->GetType(); \
  auto rtype = r.as<JSObject>()->GetType(); \
  \
  if (ltype != ObjectType::_number \
      && rtype != ObjectType::_number) \
    throw std::runtime_error("fatal: can't apply operator '" #op "'"); \
  \
  auto numobject = l.as<JSNumber>(); \
  return numobject op r; \
}

OTHER_OPERATOR(-)
OTHER_OPERATOR(*)
OTHER_OPERATOR(/)
OTHER_OPERATOR(%)

} // obj
} // grok
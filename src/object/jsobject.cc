#include "object/object.h"
#include "object/jsobject.h"


namespace grok { namespace obj {
std::string __type[7] = {"null",   "undefined", "number",  "string",
                                "object", "array",     "function"};

long long JSBasicObject::k = 0;

inline bool undefined_operation(JSBasicObject::ObjectType type) {
  return type == JSBasicObject::ObjectType::_object
          || type == JSBasicObject::ObjectType::_array
          || type == JSBasicObject::ObjectType::_undefined;
}

Object operator+(std::shared_ptr<JSNumber> l, Object r) 
{
  auto type = r.as<JSBasicObject>()->GetType();

  if (type == JSBasicObject::ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else if (type == JSBasicObject::ObjectType::_string) {
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
  auto type = r.as<JSBasicObject>()->GetType(); \
  \
  if (type == JSBasicObject::ObjectType::_number) { \
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

Object operator +(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSBasicObject>()->GetType();

  if (type == JSBasicObject::ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
    return Object(result);
  } else if (type == JSBasicObject::ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

Object operator+(Object l, Object r)
{
  auto ltype = l.as<JSBasicObject>()->GetType();
  auto rtype = r.as<JSBasicObject>()->GetType();

  if (undefined_operation(ltype) || undefined_operation(rtype)) {
    DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(+, Object);
  }

  switch (ltype) {
  case JSBasicObject::ObjectType::_string:
  {
    auto strobject = l.as<JSString>();
    return strobject + r;
  }

  case JSBasicObject::ObjectType::_number:
  {
    auto numobject = l.as<JSNumber>();
    return numobject + r;
  }

  case JSBasicObject::ObjectType::_object:
  case JSBasicObject::ObjectType::_array:
  case JSBasicObject::ObjectType::_undefined:
  default:
    throw std::runtime_error("unknown type caught in Object operator+");
  }
}

#define OTHER_OPERATOR(op) \
Object operator op (Object l, Object r) \
{ \
  auto ltype = l.as<JSBasicObject>()->GetType(); \
  auto rtype = r.as<JSBasicObject>()->GetType(); \
  \
  if (ltype != JSBasicObject::ObjectType::_number \
      && rtype != JSBasicObject::ObjectType::_number) \
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
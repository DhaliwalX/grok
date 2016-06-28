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

Object operator<(std::shared_ptr<JSNumber> l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        < rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        < rhs->ToString());
    return Object(result);
  } else {
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
  }
}

Object operator>(std::shared_ptr<JSNumber> l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        > rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        > rhs->ToString());
    return Object(result);
  } else {
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
  }
}

Object operator-(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      - rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator*(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      * rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator/(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      / rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator%(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      % rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}


Object operator<<(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      << rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator>>(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      >> rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator|(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      | rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator&(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      & rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator||(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      || rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator&&(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      && rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator^(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      ^ rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator==(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      == rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator!=(std::shared_ptr<JSNumber> l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      != rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

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

Object operator>(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        > rhs->ToString());
    return Object(result);
  } else if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

Object operator!=(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        != rhs->ToString());
    return Object(result);
  } else if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

Object operator==(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        == rhs->ToString());
    return Object(result);
  } else if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

Object operator<(std::shared_ptr<JSString> l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        < rhs->ToString());
    return Object(result);
  } else if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    return rhs + Object(l);
  } else 
    throw std::runtime_error("Code reached a place where it"
                          " was not supposed to be");
}

#define OPERATOR_FOR_STRING_NUMBER(op) \
Object operator op(Object l, Object r) \
{ \
  auto ltype = l.as<JSObject>()->GetType(); \
 \
  switch (ltype) { \
  case ObjectType::_string: \
  { \
    auto strobject = l.as<JSString>(); \
    return strobject op r; \
  } \
 \
  case ObjectType::_number: \
  { \
    auto numobject = l.as<JSNumber>(); \
    return numobject op r; \
  } \
 \
  case ObjectType::_object: \
  case ObjectType::_array: \
  case ObjectType::_undefined: \
  default: {\
      auto result = std::make_shared<UndefinedObject>(); \
      return Object(result); \
    }  \
  } \
}

OPERATOR_FOR_STRING_NUMBER(+)
OPERATOR_FOR_STRING_NUMBER(<)
OPERATOR_FOR_STRING_NUMBER(>)
OPERATOR_FOR_STRING_NUMBER(==)
OPERATOR_FOR_STRING_NUMBER(!=)

#define OTHER_OPERATOR(op) \
Object operator op (Object l, Object r) \
{ \
  auto ltype = l.as<JSObject>()->GetType(); \
  auto rtype = r.as<JSObject>()->GetType(); \
  \
  if (ltype != ObjectType::_number \
      && rtype != ObjectType::_number) {\
      auto result = std::make_shared<UndefinedObject>(); \
      return Object(result); \
  }  \
  \
  auto numobject = l.as<JSNumber>(); \
  return numobject op r; \
}

OTHER_OPERATOR(-)
OTHER_OPERATOR(*)
OTHER_OPERATOR(/)
OTHER_OPERATOR(%)
OTHER_OPERATOR(>>)
OTHER_OPERATOR(<<)
OTHER_OPERATOR(&)
OTHER_OPERATOR(|)
OTHER_OPERATOR(||)
OTHER_OPERATOR(&&)
OTHER_OPERATOR(^)

} // obj
} // grok
#include "object/object.h"
#include "object/jsobject.h"
#include "object/function.h"
#include "object/jsnumber.h"

namespace grok { namespace obj {
std::string __type[7] = {"null",   "undefined", "number",  "string",
                                "object", "array",     "function"};

inline bool undefined_operation(ObjectType type) {
  return type == ObjectType::_object
          || type == ObjectType::_array
          || type == ObjectType::_undefined
          || type == ObjectType::_null;
}

Object operator+(std::shared_ptr<JSNumber> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
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

Object operator<(std::shared_ptr<JSNumber> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        < rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        < rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr < rstr);
  }
}

Object operator>(std::shared_ptr<JSNumber> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        > rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        > rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr > rstr);
  }
}

Object operator<=(std::shared_ptr<JSNumber> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        <= rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        <= rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr <= rstr);
  }
}

Object operator>=(std::shared_ptr<JSNumber> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        >= rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        >= rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr >= rstr);
  }
}

Object operator-(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      - rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        - rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator*(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      * rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        * rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator/(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      / rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        / rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator%(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      % rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        % (int32_t)rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator<<(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      << rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        << (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator>>(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      >> (int32_t)rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        >> (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator|(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      | rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        | (int32_t)rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator&(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      & (int32_t)rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        & (int32_t  )rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator||(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      || rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        || rhs->GetNumber());
    return Object(result);
  } else {
      auto obj = r.as<JSObject>();
      auto result = CreateJSNumber(l->GetNumber() || obj->IsTrue());
      return *(result);
  }
}

Object operator&&(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      && rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        && rhs->GetNumber());
    return Object(result);
  } else {
      auto obj = r.as<JSObject>();
      auto result = CreateJSNumber(l->GetNumber() && obj->IsTrue());
      return *(result);
  }
}

Object operator^(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      ^ (int32_t)rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        ^ (int32_t)rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return (result);
  }
}

Object operator==(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      == rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        == rhs->GetNumber());
    return Object(result);
  } else {
      auto result = CreateJSNumber(0);
      return *(result);
  }
}

Object operator!=(std::shared_ptr<JSNumber> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      != rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        != rhs->GetNumber());
    return Object(result);
  } else {
      auto result = CreateJSNumber(0);
      return *result;
  }
}

///=====------------------------------------------------------------------=====
///             Operators for double's
///=====------------------------------------------------------------------=====

Object operator+(std::shared_ptr<JSDouble> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        + rhs->GetNumber());
    return Object(result);
  } else {
    auto rhs = r.as<JSObject>();
    auto result = std::make_shared<JSString>(l->ToString()
                        + rhs->ToString());
    return Object(result);
  }
}

Object operator<(std::shared_ptr<JSDouble> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        < rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        < rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr < rstr);
  }
}

Object operator>(std::shared_ptr<JSDouble> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        > rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        > rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr > rstr);
  }
}

Object operator<=(std::shared_ptr<JSDouble> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        <= rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        <= rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr <= rstr);
  }
}

Object operator>=(std::shared_ptr<JSDouble> &l, Object r) 
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        >= rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        >= rhs->GetNumber());
    return Object(result);
  } else {
    auto obj = r.as<JSObject>();
    auto rstr = obj->ToString();
    auto lstr = l->ToString();
    return *CreateJSNumber(lstr >= rstr);
  }
}

Object operator-(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                      - rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        - rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator*(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                      * rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        * rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator/(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                      / rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSDouble>(l->GetNumber()
                        / rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator%(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      % rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        % (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}


Object operator<<(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      << rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        << (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator>>(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      >> rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        >> (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator|(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      | rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        | (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator&(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      & rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        & (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = std::make_shared<UndefinedObject>();
      return Object(result);
  }
}

Object operator||(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      || rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        || (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto obj = r.as<JSObject>();
      auto result = CreateJSNumber(l->GetNumber() || obj->IsTrue());
      return *(result);
  }
}

Object operator&&(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      && rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        && (int32_t)rhs->GetNumber());
    return Object(result);
  } else  {
      auto obj = r.as<JSObject>();
      auto result = CreateJSNumber(l->GetNumber() && obj->IsTrue());
      return *(result);
  }
}

Object operator^(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                      ^ rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>((int32_t)l->GetNumber()
                        ^ (int32_t)rhs->GetNumber());
    return Object(result);
  } else {
      auto result = std::make_shared<UndefinedObject>();
      return (result);
  }
}

Object operator==(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      == rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        == rhs->GetNumber());
    return Object(result);
  } else {
      auto result = CreateJSNumber(0);
      return *(result);
  }
}

Object operator!=(std::shared_ptr<JSDouble> &l, Object r) 
{ 
  auto type = r.as<JSObject>()->GetType();
  if (type == ObjectType::_number) {
    auto rhs = r.as<JSNumber>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                      != rhs->GetNumber());
    return Object(result);
  } else if (type == ObjectType::_double) {
    auto rhs = r.as<JSDouble>();
    auto result = std::make_shared<JSNumber>(l->GetNumber()
                        != rhs->GetNumber());
    return Object(result);
  } else  {
      auto result = CreateJSNumber(0);
      return *result;
  }
}

///=====------------------------------------------------------------------=====
///             Operators for strings
///=====------------------------------------------------------------------=====

Object operator +(std::shared_ptr<JSString> &l, Object r)
{
    auto obj = r.as<JSObject>();
    return *CreateJSString(l->ToString() + obj->ToString());
}

Object operator>(std::shared_ptr<JSString> &l, Object r)
{
    auto obj = r.as<JSObject>();
    return *CreateJSNumber(l->ToString() > obj->ToString());
}

Object operator<(std::shared_ptr<JSString> &l, Object r)
{
    auto obj = r.as<JSObject>();
    return *CreateJSNumber(l->ToString() > obj->ToString());
}

Object operator>=(std::shared_ptr<JSString> &l, Object r)
{
    auto obj = r.as<JSObject>();
    return *CreateJSNumber(l->ToString() >= obj->ToString());
}

Object operator<=(std::shared_ptr<JSString> &l, Object r)
{
    auto obj = r.as<JSObject>();
    return *CreateJSNumber(l->ToString() <= obj->ToString());
}

Object operator!=(std::shared_ptr<JSString> &l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        != rhs->ToString());
    return Object(result);
  } else {
    return *CreateJSNumber(0);
  }
}

Object operator==(std::shared_ptr<JSString> &l, Object r)
{
  auto type = r.as<JSObject>()->GetType();

  if (type == ObjectType::_string) {
    auto rhs = r.as<JSString>();
    auto result = std::make_shared<JSNumber>(l->ToString()
                        == rhs->ToString());
    return Object(result);
  } else 
    return *CreateJSNumber(0);  
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
  case ObjectType::_double: \
  { \
    auto numobject = l.as<JSDouble>(); \
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

#define SPECIAL_OPERATORS(op) \
Object operator op(Object l, Object r) \
{ \
  auto ltype = l.as<JSObject>()->GetType(); \
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
  case ObjectType::_double: \
  { \
    auto numobject = l.as<JSDouble>(); \
    return numobject op r; \
  } \
  default: \
  { \
    auto obj = l.as<JSObject>();\
    auto str = CreateJSString(obj->ToString())->as<JSString>();\
    return str op r;\
  } \
  } \
}

SPECIAL_OPERATORS(+)
SPECIAL_OPERATORS(<)
SPECIAL_OPERATORS(>)
SPECIAL_OPERATORS(||)
SPECIAL_OPERATORS(&&)
SPECIAL_OPERATORS(<=)
SPECIAL_OPERATORS(>=)
OPERATOR_FOR_STRING_NUMBER(==)
OPERATOR_FOR_STRING_NUMBER(!=)

#define OTHER_OPERATOR(op) \
Object operator op (Object l, Object r) \
{ \
  auto ltype = l.as<JSObject>()->GetType(); \
  \
  switch (ltype) {  \
  case ObjectType::_number: \
  { \
    auto numobject = l.as<JSNumber>(); \
    return numobject op r; \
  } \
  case ObjectType::_double: \
  { \
    auto numobject = l.as<JSDouble>(); \
    return numobject op r; \
  } \
  default: \
    break;\
  }; \
  \
  auto object = std::make_shared<UndefinedObject>(); \
  return Object(object); \
}

OTHER_OPERATOR(-)
OTHER_OPERATOR(*)
OTHER_OPERATOR(/)
OTHER_OPERATOR(%)
OTHER_OPERATOR(>>)
OTHER_OPERATOR(<<)
OTHER_OPERATOR(&)
OTHER_OPERATOR(|)
OTHER_OPERATOR(^)

} // obj
} // grok
#include "object/jsnumber.h"

#include <sstream>

namespace grok {
namespace obj {

std::string JSDouble::ToString() const
{
     std::ostringstream os;
     os << number_;
     return os.str();
}

std::string JSDouble::AsString() const
{
    return ToString();
}

std::shared_ptr<Object> CreateJSNumber(std::string str)
{
    try {
      auto S = std::make_shared<JSDouble>(str);
      auto W = std::make_shared<Object>(S);
      return W;
    } catch (...) {
      return CreateUndefinedObject();
    }
}

std::shared_ptr<Object> CreateJSNumber(double num)
{
    auto S = std::make_shared<JSDouble>(num);
    auto W = std::make_shared<Object>(S);
    return W;
}

}
}

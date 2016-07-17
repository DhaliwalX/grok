#include "libs/string/split.h"
#include "object/function.h"

#include <regex>

namespace grok {
namespace libs {
using namespace grok::obj;

std::shared_ptr<Object> StringSplit(std::shared_ptr<Argument> Args)
{
    // get the string on which we are working
    auto This = Args->GetProperty("this");

}

}
}


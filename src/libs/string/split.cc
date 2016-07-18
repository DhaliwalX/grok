#include "libs/string/split.h"
#include "object/function.h"

#include <regex>

namespace grok {
namespace libs {
using namespace grok::obj;

// TODO: to complete 
std::shared_ptr<Object> StringSplit(std::shared_ptr<Argument> Args)
{
    // get the string on which we are working
    auto This = Args->GetProperty("this");

    return CreateUndefinedObject(); // shut up the compiler
}

}
}


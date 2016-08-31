#include "libs/string/split.h"
#include "object/function.h"
#include "object/array.h"

#include <regex>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace grok {
namespace libs {
using namespace grok::obj;

// TODO: to complete 
std::shared_ptr<Object> StringSplit(std::shared_ptr<Argument> Args)
{
    // get the string on which we are working
    auto This = Args->GetProperty("this");
    auto pattern = Args->GetProperty("pattern");

    if (!IsJSString(pattern)) {
        return CreateUndefinedObject();
    }

    auto &patternString = pattern->as<JSString>()->str();
    auto &hostString = This->as<JSString>()->str();

    std::shared_ptr<JSArray> arr = std::make_shared<JSArray>();
    std::vector<std::string> results;

    boost::algorithm::split(results, hostString, boost::is_any_of(patternString));

    for (auto &str : results) {
        arr->Push(CreateJSString(str));
    }
    return std::make_shared<Handle>(arr);
}

}
}


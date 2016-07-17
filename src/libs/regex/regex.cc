#include "libs/regex/regex.h"

#include "object/function-template.h"
#include "object/array.h"
#include <functional>

namespace grok {
namespace libs {

using namespace grok::obj;

class RegExpError : public JSError {
public:
    RegExpError(std::string msg = "")
        : JSError("RegExpError", msg)
    { }

    const char *what() const noexcept override
    {
        return JSError::what();
    }
};

Regex::Regex()
    : reg_{ }, flags_{ RegexFlags::reg_reg }
{
    Init("");
}

Regex::Regex(std::string reg, int32_t fgs = RegexFlags::reg_reg)
    : reg_ { reg }, flags_ { fgs }
{
    Init(reg);
}

void Regex::Finalize(std::string::size_type pos)
{
    if (!(flags_ & reg_global)) {
        return;
    }

    last_index_->as<JSNumber>()->GetNumber() = pos;
}

bool Regex::Execute(std::shared_ptr<JSArray> result,
    std::string &subject)
{
    bool match_found = false;
    std::smatch pieces;

    match_found = std::regex_match(subject, pieces, reg_);
    if (!match_found || !result)
        return match_found;

    for (auto &match :  pieces) {
        std::string s = match.str();
        result->Push(CreateJSString(s));
    }
    auto pos = subject.find(pieces[0].str());
    result->AddProperty("index", CreateJSNumber(pos));
    result->AddProperty("input", CreateJSString(subject));

    // get the last index
    pos = pieces[0].str().length() - 1;
    Finalize(pos);
    return match_found;
}

std::shared_ptr<Handle> Regex::Exec(std::shared_ptr<Argument> args)
{
    auto subject = args->GetProperty("subject");
    auto result_wrapped = CreateArray(0);
    auto result = result_wrapped->as<JSArray>();
    auto subject_string = subject->as<JSObject>()->ToString();
    if (Execute(result, subject_string));
        return result_wrapped;
    return CreateJSNull();
}

std::shared_ptr<Handle> Regex::Test(std::shared_ptr<Argument> args)
{
    auto subject = args->GetProperty("subject");
    auto subject_string = subject->as<JSObject>()->ToString();
    auto result = Execute(nullptr, subject_string);
    return CreateJSNumber(result);
}

void Regex::Init(std::string src)
{
    auto temp = CreateFunction(std::bind(&Regex::Exec, this, 
        std::placeholders::_1));
    AddProperty("exec", temp);
    temp = CreateFunction(std::bind(&Regex::Test, this,
        std::placeholders::_1));
    AddProperty("test", temp);
    last_index_ = CreateJSNumber(0);
    AddProperty("lastIndex", last_index_);
    source_ = CreateJSString(src);
    AddProperty("source", source_);

    if (flags_ & reg_reg) {
        AddProperty("global", CreateJSNumber(0));
    } else {
        AddProperty("global", CreateJSNumber(1));
    }
    if (flags_ & reg_ignore_case) {
        AddProperty("ignoreCase", CreateJSNumber(1));
        reg_.assign(src, std::regex_constants::ECMAScript | 
                std::regex_constants::icase);
    } else {
        AddProperty("ignoreCase", CreateJSNumber(0));
    }
}

std::shared_ptr<Handle> CreateRegExp(std::string reg, int32_t flags)
{
    auto r = std::make_shared<Regex>(reg, flags);
    DefineInternalObjectProperties(r.get());
    return std::make_shared<Handle>(r);
}

int32_t ParseFlags(const std::string &str)
{
    int32_t flags = 0;
    if (str.find_first_of("g") != std::string::npos) {
        flags |= Regex::RegexFlags::reg_global;
    }

    if (str.find_first_of("i") != std::string::npos) {
        flags |= Regex::RegexFlags::reg_ignore_case;
    }
    return flags;
}

std::shared_ptr<Handle> RegExpCtor(std::shared_ptr<Argument> args)
{
    auto pattern = args->GetProperty("pattern");
    auto pattern_string = pattern->as<JSObject>()->ToString();
    auto flags = args->GetProperty("flags");
    if (IsUndefined(pattern)) {
        return CreateRegExp("");
    } else {
        return CreateRegExp(pattern_string);
    }
    if (IsUndefined(flags)) {
        return CreateRegExp(pattern_string);
    }
    auto f = ParseFlags(flags->as<JSObject>()->ToString());
    return CreateRegExp(pattern_string, f);
}

std::shared_ptr<Handle> CreateRegExpCtor()
{
    auto func_wrapped = CreateFunction(RegExpCtor);
    auto func = func_wrapped->as<Function>();
    func->SetParams({ "pattern", "flags" });
    DefineInternalObjectProperties(func.get());
    // DefineFunctionProperties();
    func->AddProperty("prototype", CreateRegExp(""));
    return func_wrapped;
}

}
}

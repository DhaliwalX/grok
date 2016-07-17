#include "libs/string/properties.h"
#include "object/function.h"
#include "libs/string/slice.h"

#include <boost/algorithm/string.hpp>

namespace grok {
namespace libs {
using namespace grok::obj;

// returns pair of int and bool, bool is true if typeof obj === number
std::pair<int32_t, bool> ParseIndex(std::shared_ptr<Handle> obj)
{
    if (!IsJSNumber(obj)) {
        return { 0, false };
    }
    int32_t res = obj->as<JSDouble>()->GetNumber();
    return { res, true };
}

std::shared_ptr<Handle> StringCharAt(std::shared_ptr<Argument> args)
{
    auto js_this = args->GetProperty("this");
    auto js_string = js_this->as<JSObject>()->ToString();

    auto idx = args->GetProperty("index");

    if (!IsJSNumber(idx)) {
        return CreateJSString("");
    }

    auto num = (size_t)idx->as<JSDouble>()->GetNumber();
    if (num >= js_string.length()) {
        return CreateJSString("");
    }
    std::string res(1, js_string[num]);
    return CreateJSString(res);
}

std::shared_ptr<Handle> StringCharCodeAt(std::shared_ptr<Argument> args)
{
    auto js_this = args->GetProperty("this");
    auto js_string = js_this->as<JSObject>()->ToString();

    auto idx = args->GetProperty("index");

    if (!IsJSNumber(idx)) {
        return CreateJSNumber(0);
    }

    auto num = (size_t)idx->as<JSDouble>()->GetNumber();
    if (num >= js_string.length()) {
        return CreateJSNumber(0);
    }

    // hack but works for ASCII
    return CreateJSNumber((int16_t)js_string[num]);
}

void ConcatSingleStr(JSString *str, std::shared_ptr<Object> obj)
{
    auto s = obj->as<JSObject>()->ToString();
    // else just append the element
    str->concat(s);
}

std::shared_ptr<Object> StringConcat(std::shared_ptr<Argument> Args)
{
    auto This = Args->GetProperty("this");
    auto Result =
        CreateJSString(This->as<JSObject>()->ToString())->as<JSString>();

    if (!IsJSString(This))
        return CreateUndefinedObject();

    // concat every arg
    for (auto Arg : *Args) {
        ConcatSingleStr(Result.get(), Arg);
    }

    return std::make_shared<Object>(Result);
}

std::shared_ptr<Handle> StringEndsWith(std::shared_ptr<Argument> args)
{
    auto tis = args->GetProperty("this");

    if (!IsJSString(tis))
        return CreateUndefinedObject();
    auto &tisstr = tis->as<JSString>()->GetString();
    auto start_handle = args->GetProperty("last");
    auto p = ParseIndex(start_handle);
    int32_t last = 0;

    if (!p.second) {
        last = tisstr.length();
    } else if (p.first < 0) {
        return CreateJSNumber(0);
    } else {
        last = p.first >= (int32_t)tisstr.length() ? tisstr.length() : p.first;
    }
    // TODO ::= use last variable to control the match
    (void)last;

    // get the next argument
    auto match_handle = args->GetProperty("match");
    auto match_str = match_handle->as<JSObject>()->ToString();

    auto result = boost::ends_with(tisstr, match_str);
    return CreateJSNumber(result);
}

int32_t FindFrom(std::string &str, std::string &match_str, int32_t start,
    bool from_last)
{
    std::string::size_type result;

    if (from_last) {
        result = str.rfind(match_str, str.length() - start);
    } else {
        result = str.find(match_str, start);
    }
    if (result == std::string::npos)
        return -1;
    return result;
}

int32_t IndexOfInternal(std::shared_ptr<Argument> &args, bool from_last)
{
    auto tis = args->GetProperty("this");

    if (!IsJSString(tis))
        return -1;
    auto &tisstr = tis->as<JSString>()->GetString();
    auto start_handle = args->GetProperty("start");
    auto p = ParseIndex(start_handle);
    int32_t start = 0;

    if (!p.second || p.first < 0) {
        start = 0;
    } else if (p.first >= (int32_t)tisstr.length()) {
        return -1;
    } else {
        start = p.first;
    }
    // get the next argument
    auto match_handle = args->GetProperty("match");
    auto match_str = match_handle->as<JSObject>()->ToString();
    return (FindFrom(tisstr, match_str, start, from_last));
}

std::shared_ptr<Handle> StringIndexOf(std::shared_ptr<Argument> args)
{
    auto res = IndexOfInternal(args, false);
    return CreateJSNumber(res);
}

std::shared_ptr<Handle> StringIncludes(std::shared_ptr<Argument> args)
{
    auto res = IndexOfInternal(args, false);

    return res < 0 ? CreateJSNumber(0) : CreateJSNumber(1);
}

std::shared_ptr<Handle> StringLastIndexOf(std::shared_ptr<Argument> args)
{
    auto res = IndexOfInternal(args, true);
    return CreateJSNumber(res);
}

std::shared_ptr<Handle> StringUpperCase(std::shared_ptr<Argument> args)
{
    auto this_str_handle = args->GetProperty("this");
    if (!IsJSString(this_str_handle))
        return CreateUndefinedObject();
    auto this_str = this_str_handle->as<JSString>();
    auto result = boost::to_upper_copy(this_str->GetString());
    return CreateJSString(result);
}

std::shared_ptr<Handle> StringLowerCase(std::shared_ptr<Argument> args)
{
    auto this_str_handle = args->GetProperty("this");
    if (!IsJSString(this_str_handle))
        return CreateUndefinedObject();
    auto this_str = this_str_handle->as<JSString>();
    auto result = boost::to_lower_copy(this_str->GetString());
    return CreateJSString(result);
}

std::shared_ptr<Handle> StringTrim(std::shared_ptr<Argument> args)
{
    auto this_str_handle = args->GetProperty("this");
    if (!IsJSString(this_str_handle))
        return CreateUndefinedObject();
    auto this_str = this_str_handle->as<JSString>();
    auto result = boost::trim_copy(this_str->GetString());
    return CreateJSString(result);
}

std::shared_ptr<Handle> StringTrimLeft(std::shared_ptr<Argument> args)
{
    auto this_str_handle = args->GetProperty("this");
    if (!IsJSString(this_str_handle))
        return CreateUndefinedObject();
    auto this_str = this_str_handle->as<JSString>();
    auto result = boost::trim_left_copy(this_str->GetString());
    return CreateJSString(result);
}

std::shared_ptr<Handle> StringTrimRight(std::shared_ptr<Argument> args)
{
    auto this_str_handle = args->GetProperty("this");
    if (!IsJSString(this_str_handle))
        return CreateUndefinedObject();
    auto this_str = this_str_handle->as<JSString>();
    auto result = boost::trim_right_copy(this_str->GetString());
    return CreateJSString(result);
}

void DefineStringProperties(grok::obj::JSString *str)
{
    auto P = grok::obj::CreateFunction(StringSlice);
    auto PF = P->as<grok::obj::Function>();
    PF->SetParams({ "start", "end" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("slice", P);
    // slice is same as substr
    str->AddProperty("substr", P);

    P = grok::obj::CreateFunction(StringCharAt);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "index" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("charAt", P);

    P = grok::obj::CreateFunction(StringCharCodeAt);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "index" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("charCodeAt", P);
    str->AddProperty("codePointAt", P);

    P = grok::obj::CreateFunction(StringConcat);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("concat", P);

    P = grok::obj::CreateFunction(StringEndsWith);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "match", "last" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("endsWith", P);

    P = grok::obj::CreateFunction(StringIndexOf);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "match", "start" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("indexOf", P);

    P = grok::obj::CreateFunction(StringIncludes);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "match", "start" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("includes", P);

    P = grok::obj::CreateFunction(StringLastIndexOf);
    PF = P->as<grok::obj::Function>();
    PF->SetParams({ "match", "start" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("lastIndexOf", P);

    P = grok::obj::CreateFunction(StringTrim);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("trim", P);

    P = grok::obj::CreateFunction(StringTrimLeft);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("trimLeft", P);

    P = grok::obj::CreateFunction(StringTrimRight);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("trimRight", P);

    P = grok::obj::CreateFunction(StringUpperCase);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("toUpperCase", P);

    P = grok::obj::CreateFunction(StringLowerCase);
    PF = P->as<grok::obj::Function>();
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("toLowerCase", P);
}

}
}

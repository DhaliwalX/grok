#ifndef REGEX_H_
#define REGEX_H_

#include "object/jsbasicobject.h"
#include "object/argument.h"
#include "object/jsstring.h"
#include <regex>

namespace grok {
namespace libs {

// A basic regex class
class Regex : public grok::obj::JSObject {
public:
    enum RegexFlags {
        reg_reg = 0,
        reg_ignore_case = 1,
        reg_global = 2
    };
    Regex(std::string reg, int32_t fgs);
    Regex();

    void Finalize(std::string::size_type pos);
    bool Execute(std::shared_ptr<grok::obj::JSArray> arr,
        std::string &sub);
    std::shared_ptr<grok::obj::Handle>
        Exec(std::shared_ptr<grok::obj::Argument>);
    std::shared_ptr<grok::obj::Handle>
        Test(std::shared_ptr<grok::obj::Argument>);

    void Init(std::string src);
private:
    std::regex reg_;

    // just for faster access
    std::shared_ptr<grok::obj::Handle> last_index_;
    std::shared_ptr<grok::obj::Handle> source_;
    int32_t flags_;
};

extern std::shared_ptr<grok::obj::Handle> CreateRegExp(std::string reg,
    int32_t flags = Regex::RegexFlags::reg_reg);
extern std::shared_ptr<grok::obj::Handle> RegExpCtor(
    std::shared_ptr<grok::obj::Argument> args);
extern std::shared_ptr<grok::obj::Handle> CreateRegExpCtor();

}
}

#endif

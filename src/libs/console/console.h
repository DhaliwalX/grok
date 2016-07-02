#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "object/function.h"
#include <map>
#include <string>

namespace grok {
namespace libs {

class Console : public grok::obj::JSObject {
public:
    Console();

    static std::shared_ptr<grok::obj::Object>
    Log(std::shared_ptr<grok::obj::Argument> Args);

    static std::shared_ptr<grok::obj::Object>
    Error(std::shared_ptr<grok::obj::Argument> Args);

    static std::shared_ptr<grok::obj::Object>
    Print(std::shared_ptr<grok::obj::Argument> Args);
    
    static std::shared_ptr<grok::obj::Object>
    Flush(std::shared_ptr<grok::obj::Argument> Args);
};

}
}

#endif

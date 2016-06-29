#include "libs/console.h"

#include <functional>

using namespace std::placeholders;

namespace grok {
namespace libs {

Console::Console()
{
    auto log = grok::obj::CreateFunction(Console::Log);
    auto error = grok::obj::CreateFunction(Console::Error);

    this->AddProperty("log", log);
    this->AddProperty("error", error);
}

std::shared_ptr<grok::obj::Object>
    Console::Log(std::shared_ptr<grok::obj::Argument> Args)
{
    if (Args->Size() == 0) {
        std::cout << std::endl;
        return grok::obj::CreateUndefinedObject();
    }
    auto it = Args->begin();
    auto end = Args->end();

    while (it != end - 1) {
        auto O = (*it)->as<grok::obj::JSObject>();
        std::cout << O->ToString() << " ";
        ++it;
    }

    auto O = (*it)->as<grok::obj::JSObject>();
    std::cout << O->ToString() << std::endl;
    return grok::obj::CreateUndefinedObject();
}

std::shared_ptr<grok::obj::Object>
    Console::Error(std::shared_ptr<grok::obj::Argument> Args)
{
    return Log(Args);
}

}
}

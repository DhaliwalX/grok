#include "library.h"

#include "libs/console.h" // console.log, console.error 

namespace grok {
namespace libs {
using namespace grok::vm;
using namespace grok::obj;

int LoadLibraries(VMContext *ctx)
{
    // just console for now
    auto V = GetVStore(ctx);

    auto C = std::make_shared<Console>();
    auto Wrapped = Value(std::make_shared<Object>(C));
    V->StoreValue("console", Wrapped);

    return 0;
}

}
}

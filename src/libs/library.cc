#include "library.h"

#include "libs/console/console.h" // console.log, console.error 
#include "libs/array/array_constructor.h"
#include "libs/example/example.h"

namespace grok {
namespace libs {
using namespace grok::vm;
using namespace grok::obj;

int LoadLibraries(VMContext *ctx)
{
    // just console for now
    auto V = GetVStore(ctx);

    auto C = std::make_shared<Console>();
    DefineInternalObjectProperties(C.get());
    C->SetNonWritable();
    auto Wrapped = Value(std::make_shared<Object>(C));
    V->StoreValue("console", Wrapped);

    auto array_ctor = CreateArrayConstructorObject();
    V->StoreValue("Array", array_ctor);

    // create an example object constructor
    auto ex = example::Example::CreateConstructor();
    V->StoreValue("Example", ex);
    return 0;
}

}
}

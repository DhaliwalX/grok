#include "libs/string/properties.h"
#include "object/function.h"
#include "libs/string/slice.h"

namespace grok {
namespace libs {

void DefineStringProperties(grok::obj::JSString *str)
{
    auto P = grok::obj::CreateFunction(StringSlice);
    auto PF = P->as<grok::obj::Function>();
    PF->SetParams({ "start", "end" });
    PF->SetNonWritable();
    PF->SetNonEnumerable();
    str->AddProperty("slice", P);
}

}
}

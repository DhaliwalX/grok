#include "vm/var-store.h"

namespace grok {
namespace vm {

VStore::VStore()
    : MV{}
{ }

Value VStore::GetValue(const std::string &N)
{
    auto Result = MV.find(N);

    if (Result == MV.end())
        throw std::runtime_error(std::string() 
            + "variable '" + N + "' doesn't exists");

    return Result->second;
}

void VStore::StoreValue(const std::string &N, Value V)
{
    MV[N] = V;
}

} // vm
} // grok

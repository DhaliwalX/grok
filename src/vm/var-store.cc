#include "vm/var-store.h"

namespace grok {
namespace vm {

VStore::VStore()
    : MV{}, VS{}
{
    MV = std::make_shared<MappedValues>();
}

Value VStore::GetValue(const std::string &N)
{
    auto Result = MV->find(N);

    if (Result == MV->end()) {
        return TryForOtherScopes(N);
    }

    return Result->second;
}

void VStore::StoreValue(const std::string &N, Value V)
{
    MV->insert({ N, V });
}

void VStore::CreateScope()
{
    VS.Push(MV);
    MV = std::make_shared<MappedValues>();
}

void VStore::RemoveScope()
{
    MV = VS.Pop();
}

Value VStore::TryForOtherScopes(const std::string &Name)
{
    for (auto i = VS.rbegin(); i != VS.rend(); i++) {
        auto Result = (*i)->find(Name);

        if (Result != (*i)->end())
            return Result->second;
    }

    throw std::runtime_error(std::string() + "no variable named '"
        + Name + "'");
}

} // vm
} // grok

#include "vm/var-store.h"
#include "common/exceptions.h"
namespace grok {
namespace vm {

VStore::VStore()
    : MV{}, VS{}
{
    MV = std::make_shared<MappedValues>();
    VS.push_back(MV);
}

Value VStore::GetValue(const std::string &N)
{
    if (N == "this") {
        auto W = (This());
        return Value(W);
    }
    if (MV->HasProperty(N))
        return Value(MV->GetProperty(N));

    return TryForOtherScopes(N);
}

void VStore::StoreValue(const std::string &N, Value V)
{
    MV->AddProperty(N, V.O);
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
        if ((*i)->HasProperty(Name))
            return Value((*i)->GetProperty(Name));
    }

    throw ReferenceError(std::string() + "no variable named '"
        + Name + "'");
}

bool VStore::HasValue(const std::string &name)
{
    if (MV->HasProperty(name))
        return true;
    for (auto i = VS.rbegin(); i != VS.rend(); i++) {
        if ((*i)->HasProperty(name))
            return true;
    }
    return false;
}

} // vm
} // grok

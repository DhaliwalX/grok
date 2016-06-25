#include "vm/context.h"

namespace grok {
namespace vm {
VMContext::VMContext()
    : VS{}
{ }

VStore *VMContext::GetVStore()
{
    return VS.get();
}

void VMContext::SetVStore(VStore* ptr)
{
    VS.reset(ptr);
}

VStore *GetVStore(VMContext *context)
{
    return context->GetVStore();
}

VMContext *GetGlobalVMContext()
{
    return &Gcontext;
}

void InitializeVMContext()
{
    GetGlobalVMContext()->SetVStore(new VStore{});
}

VMContext Gcontext;

} // vm
} // grok

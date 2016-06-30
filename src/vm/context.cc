#include "vm/context.h"
#include "libs/library.h"

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

VM* VMContext::GetVM()
{
    return vm;
}

void VMContext::SetVM(VM *v)
{
    vm = v;
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
    grok::libs::LoadLibraries(GetGlobalVMContext());
}

VMContext Gcontext;

} // vm
} // grok

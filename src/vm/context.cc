#include "vm/context.h"
#include "grok/context.h"
#include "libs/library.h"
#include "vm/vm.h"

namespace grok {
namespace vm {
VMContext::VMContext()
    : VS{}, vm { nullptr }
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
    return grok::GetContext()->GetVMContext();
}

void InitializeVMContext()
{
    GetGlobalVMContext()->SetVStore(new VStore{});
    grok::libs::LoadLibraries(GetGlobalVMContext());
    auto vm = CreateVM(GetGlobalVMContext());
    GetGlobalVMContext()->SetVM(vm.release());
}

void TearDownVMContext()
{
    GetGlobalVMContext()->SetVStore(nullptr);
}

} // vm
} // grok

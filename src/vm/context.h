#ifndef VM_CONTEXT_H_
#define VM_CONTEXT_H_

#include "vm/var-store.h"
#include "common/util.h"

namespace grok {
namespace vm {

class VM;
//====-------------------------------------------------------------------====
// Defines Global Context for Virtual Machine
//====-------------------------------------------------------------------====
class VMContext {
    DELETE_COPY_AND_MOVE_CONSTRUCTORS(VMContext)
public:
    VMContext();

    /// GetVStore ::= returns the VStore object VS
    VStore *GetVStore();

    /// GetVM ::= returns the VM
    VM* GetVM();

    void SetVM(VM *v);

    /// SetVStore ::= sets the VS
    void SetVStore(VStore *ptr);
private:
    std::unique_ptr<VStore> VS;
    VM *vm;
};

extern VMContext Gcontext;

/// InitializeVMContext ::= initializes the Gcontext
extern void InitializeVMContext();

/// GetGlobalContext ::= returns pointer to Gcontext
extern VMContext *GetGlobalVMContext();

/// GetVStore ::= returns the VStore object from context
extern VStore *GetVStore(VMContext *context);

} // vm
} // grok
#endif // context.h

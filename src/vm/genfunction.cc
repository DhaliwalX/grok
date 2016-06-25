#include "object/object.h"
#include "object/jsobject.h"
#include "object/function.h"
#include "parser/functionstatement.h"
#include "vm/instruction-builder.h"
#include "vm/instruction.h"
#include "vm/context.h"
#include "vm/vm.h"

namespace grok {
namespace parser {

using namespace grok::vm;
using namespace grok::obj;

/// emits code for function definition
/// We don't generate the code for the body now but defer it until
/// it is required i.e. when a call is placed to this function during
/// execution
void FunctionStatement::emit(std::shared_ptr<InstructionBuilder>)
{
    std::string Name = proto_->GetName();
    auto F = std::make_shared<Function>(std::move(body_), std::move(proto_));
    auto VS = GetVStore(GetGlobalVMContext());
    auto V = Value{};
    V.O = std::make_shared<Object>(F);
    VS->StoreValue(Name, V);
}

void FunctionPrototype::emit(std::shared_ptr<InstructionBuilder>)
{
    // do nothing
}

} // parser
} // grok

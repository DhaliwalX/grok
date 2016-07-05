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
void FunctionStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{
    std::string Name = proto_->GetName();
    auto F = CreateFunction(std::move(body_), std::move(proto_));
    auto VS = GetVStore(GetGlobalVMContext()); 

    if (Name.length()) {
        auto V = Value{};
        V.O = (F);
        VS->StoreValue(Name, V);
    }

    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_obj;
    instr->data_ = (F);

    builder->AddInstruction(std::move(instr));
}

void FunctionPrototype::emit(std::shared_ptr<InstructionBuilder>)
{
    // do nothing
}

} // parser
} // grok

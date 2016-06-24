#include "parser/functionstatement.h"
#include "vm/instruction-builder.h"
#include "vm/instruction.h"

namespace grok {
namespace parser {

using namespace grok::vm;
/// emits code for function
void FunctionStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{

}

void FunctionPrototype::emit(std::shared_ptr<InstructionBuilder> builder)
{
    
}

} // parser
} // grok

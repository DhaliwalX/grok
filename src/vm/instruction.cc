#include "vm/instruction.h"
#include "common/exceptions.h"
#include "object/jsobject.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace grok {
namespace vm {

const char *instr_to_string[] = {
#define INSTRUCTION_OP(instr) #instr
    INSTRUCTION_LIST(INSTRUCTION_OP)
#undef INSTRUCTION_OP
};

const char *type_to_string[] = {
#define DATA_TYPE_OP(type) #type
    DATA_TYPE_LIST(DATA_TYPE_OP)
#undef DATA_TYPE_OP
};

/// InstrDataTypeToString ::= returns the string representation of the 
/// instruction embedded data. It can be null, number or string only
std::string InstrDataToString(const RawInstruction<int> &instr)
{
    switch (instr.GetDataType()) {
    default:
        throw std::runtime_error("fatal: unknown data type in the instruction");

    case Datatypes::d_null:
        return "Null";

    case Datatypes::d_num:
        return std::to_string(instr.GetNumber());

    case Datatypes::d_str:
        return instr.GetString();
    }
}

std::string InstructionToString(const RawInstruction<int> &instr)
{
    std::ostringstream out;
    out << std::left << std::setw(8) << instr_to_string[instr.kind_];
    out << std::setw(6) << type_to_string[instr.data_type_];
    out << InstrDataToString(instr);
    return out.str();
}

}   // vm
}   // grok

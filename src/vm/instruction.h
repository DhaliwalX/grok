#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "object/object.h"
#include <cctype>
#include <vector>

namespace grok {
namespace vm {

#define INSTRUCTION_LIST(op)    \
    op(noop),   \
    op(fetch),   \
    op(store),   \
    op(markst),   \
    op(push),   \
    op(pushim),   \
    op(pushthis),   \
    op(poprop),   \
    op(replprop),   \
    op(index),   \
    op(res),   \
    op(news),   \
    op(cpya),   \
    op(maps),   \
    op(inc),   \
    op(dec),   \
    op(pinc),   \
    op(pdec),   \
    op(lts),   \
    op(gts),   \
    op(eqs),   \
    op(neqs),   \
    op(adds),   \
    op(subs),   \
    op(muls),   \
    op(divs),   \
    op(shls),   \
    op(shrs),   \
    op(rems),   \
    op(bors),   \
    op(bands),   \
    op(ors),   \
    op(ands),   \
    op(xors),   \
    op(loopz),   \
    op(jmp),   \
    op(call),   \
    op(ret),    \
    op(jmpz),   \
    op(leave)

#define DATA_TYPE_LIST(op)  \
    op(d_null), \
    op(d_bool), \
    op(d_num),  \
    op(d_str),  \
    op(d_name)

enum Instructions {
#define INSTRUCTION_OP(instr) instr
INSTRUCTION_LIST(INSTRUCTION_OP)
#undef INSTRUCTION_OP
};

enum Datatypes {
#define DATA_TYPE_OP(type) type
    DATA_TYPE_LIST(DATA_TYPE_OP)
#undef DATA_TYPE_OP
};

/// RawInstruction ::= template data structure for instruction
///     @param T ::= represents the type of address for jmp instructions
template <typename T>
struct RawInstruction {
    int32_t kind_;
    int32_t data_type_;
    bool boolean_;
    double number_;
    std::string str_;
    T jmp_addr_;
    grok::obj::Object data_;    // will be used in future

    auto GetKind() const { return kind_; }
    auto GetDataType() const { return data_type_; }
    auto GetData() const { return data_; }
    auto GetString() const { return str_; }
    auto GetNumber() const { return number_; }

    bool IsKindOfJump() const
    {
        switch (static_cast<Instructions>(kind_)) {
        default:
            return false;
        case jmp:
        case jmpz:
        case loopz:
            return true;
        }
    }

    void SetJumpLength(T sz)
    {
        jmp_addr_ = sz;
    }
};

extern const char *instr_to_string[];
extern const char *type_to_string[];

extern std::string InstrDataToString(const RawInstruction<int> &instr);
extern std::string InstructionToString(const RawInstruction<int> &instr);
} // vm
} // grok

#endif

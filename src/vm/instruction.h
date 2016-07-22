#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "object/object.h"
#include <cctype>
#include <vector>

namespace grok {
namespace vm {

#define INSTRUCTION_LIST_FOR_EACH(op)    \
    op(noop, Noop)   \
    op(fetch, Fetch)   \
    op(store, Store)   \
    op(markst, Markst)   \
    op(push, Push)   \
    op(pop, Pop)    \
    op(pushim, Pushim)   \
    op(pushthis, PushThis)   \
    op(poprop, PopProp)   \
    op(replprop, ReplProp)   \
    op(index, Index)   \
    op(res, Res)   \
    op(news, News)   \
    op(cpya, CopyA)   \
    op(maps, Maps)   \
    op(inc, Inc)  \
    op(dec, Dec)   \
    op(snot, SNot)   \
    op(bnot, BNot)   \
    op(pinc, Pinc)   \
    op(pdec, Pdec)   \
    op(lts, Lts)  \
    op(ltes, Ltes)   \
    op(gts, Gts)   \
    op(gtes, Gtes)   \
    op(eqs, Eqs)   \
    op(neqs, NEqs)   \
    op(adds, Adds)   \
    op(subs, Subs)   \
    op(muls, Muls)   \
    op(divs, Divs)   \
    op(shls, Shls)   \
    op(shrs, Shrs)   \
    op(rems, Rems)   \
    op(bors, Bors)   \
    op(bands, Bands)   \
    op(ors, Ors)   \
    op(ands, Ands)   \
    op(xors, Xors)   \
    op(loopz, Loopz)   \
    op(jmp, Jmp)   \
    op(call, Call)   \
    op(ret, Ret)    \
    op(jmpz, Jmpz)   \
    op(jmpnz, Jmpnz)   \
    op(mem_call, MemCall)   \
    op(leave, Leave)

#define DATA_TYPE_LIST(op)  \
    op(d_null), \
    op(d_bool), \
    op(d_num),  \
    op(d_str),  \
    op(d_name), \
    op(d_obj)

enum Instructions {
#define INSTRUCTION_OP(instr, Class) instr,
INSTRUCTION_LIST_FOR_EACH(INSTRUCTION_OP)
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
    std::shared_ptr<grok::obj::Object> data_;

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

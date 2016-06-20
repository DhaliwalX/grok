// #ifndef BYTECODE_H_
// #define BYTECODE_H_

// #include <string>
// #include <iostream>

// #define I InstructionKind::

// typedef unsigned long ul;
// typedef long l;
// typedef unsigned int ui;
// typedef short s;

// enum AddrMod : unsigned short { _Imm, _Adr, _Reg, _Idx };

// static std::string addr_mod[] = {"Imm", "Adr", "Reg", "Idx"};

// #define INSTRUCTION_LIST(A)                                                    \
//   A(NONE),                         \
//   A(LDAI),                         \
//   A(LDADR),                        \
//   A(LDAKEY),                       \
//   A(LDAARR),                       \
//   A(LDAOBJ),                       \
//   A(LDANUM),                       \
//   A(LDASTR),                       \
//   A(STRR),                         \
//   A(STRA),                         \
//   A(RESIZEARR),                    \
//   A(PUSHI),                        \
//   A(PUSHADR),                      \
//   A(PUSHREG),                      \
//   A(OPUSHREG),                     \
//   A(OPUSHOBJ),                     \
//   A(OPUSHKEY),                     \
//   A(OPUSHARR),                     \
//   A(OPUSHOBJPROP),                 \
//   A(OPUSHARROBJ),                  \
//   A(PUSHKEY),                      \
//   A(POPADR),                       \
//   A(POPA),                         \
//   A(POPKEY),                       \
//   A(POPTOARR),                     \
//   A(POPTOOBJ),                     \
//   A(OPOPTS),                       \
//   A(OPOPTARR),                     \
//   A(OPOPTOBJ),                     \
//   A(OASSIGN),                      \
//   A(EMPTY),                        \
//   A(INC),                          \
//   A(DEC),                          \
//   A(PINC),                         \
//   A(PDEC),                         \
//   A(NEG),                          \
//   A(ADD),                          \
//   A(SUB),                          \
//   A(MUL),                          \
//   A(DIV),                          \
//   A(MOD),                          \
//   A(SHR),                          \
//   A(SHL),                          \
//   A(EQ),                           \
//   A(NEQ),                          \
//   A(LT),                           \
//   A(GT),                           \
//   A(BITAND),                       \
//   A(BITOR),                        \
//   A(BITXOR),                       \
//   A(BITNOT),                       \
//   A(OR),                           \
//   A(AND),                          \
//   A(JMP),                          \
//   A(JMPZ),                         \
//   A(CALL),                         \
//   A(CALLNATIVE),                   \
//   A(RET),                          \
//   A(RET0),                         \
//   A(HLT)

// #define INSTRUCTION(x) x

// enum class Instruction { INSTRUCTION_LIST(INSTRUCTION) };

// #undef V

// #define INSTRUCTION_STRING(a)                                                  \
//   { #a }

// static std::string _instructions_[] = {INSTRUCTION_LIST(INSTRUCTION_STRING)};

// #undef S

// class Bytecode {
// public:
//   Bytecode(Instruction instruction, const Register &value)
//       : instruction_(static_cast<ui>(instruction)), value_(value) {}

//   Bytecode(Instruction instruction)
//       : instruction_(static_cast<ui>(instruction)), value_(Register()) {}

//   Bytecode(ui instruction, const Register &value)
//       : instruction_((instruction)), value_(value) {}

//   Bytecode(ui instruction) : instruction_((instruction)), value_(Register()) {}

//   Bytecode() : instruction_(static_cast<ui>(Instruction::NONE)) {}

//   ~Bytecode() = default;

//   Bytecode(const Bytecode &rhs) = default;

//   Bytecode &operator=(const Bytecode &rhs) = default;

//   void print(std::ostream &os) const {
//     os << _instructions_[(int)instruction_] << "\t\t";
//     value_.print(os);
//   };

//   unsigned int instruction_;
//   Register value_;
// };

// // all static
// namespace B {
// typedef Instruction i;

// static Bytecode none() { return Bytecode(); }

// static Bytecode ldai(Register imm) {
//   return Bytecode((static_cast<int>(i::LDAI)), imm);
// }

// static Bytecode ldadr(Register reg) {
//   return Bytecode((static_cast<ui>(i::LDADR)), reg);
// }

// static Bytecode ldakey(Register reg) {
//   return Bytecode(static_cast<ui>(i::LDAKEY), reg);
// }

// static Bytecode poptoarr() { return Bytecode(static_cast<ui>(i::POPTOARR)); }

// static Bytecode ldaarr() { return Bytecode(static_cast<ui>(i::LDAARR)); }

// static Bytecode ldaobj() { return Bytecode(static_cast<ui>(i::LDAOBJ)); }

// static Bytecode ldanum() { return Bytecode(static_cast<ui>(i::LDANUM)); }

// static Bytecode ldastr() { return Bytecode(static_cast<ui>(i::LDASTR)); }

// static Bytecode strr(Register adr) {
//   return Bytecode(static_cast<ui>(i::STRR), adr);
// }

// static Bytecode resizearr(Register adr) {
//   return Bytecode(static_cast<ui>(i::RESIZEARR), adr);
// }

// static Bytecode pushi(Register imm) {
//   return Bytecode(static_cast<ui>(i::PUSHI), imm);
// }

// static Bytecode pushadr(Register imm) {
//   return Bytecode(static_cast<ui>(i::PUSHADR), imm);
// }

// static Bytecode pushreg() { return Bytecode(static_cast<ui>(i::PUSHREG)); }

// static Bytecode opushreg() { return Bytecode(static_cast<ui>(i::OPUSHREG)); }

// static Bytecode pushkey(Register reg) {
//   return Bytecode(static_cast<ui>(i::PUSHKEY), reg);
// }

// static Bytecode popadr(Register adr) {
//   return Bytecode(static_cast<ui>(i::POPADR), adr);
// }

// static Bytecode popkey(Register key) {
//   return Bytecode(static_cast<ui>(i::POPKEY), key);
// }

// static Bytecode poptoobj(Register key) {
//   return Bytecode(static_cast<ui>(i::POPTOOBJ), key);
// }

// static Bytecode opushobj() { return Bytecode(static_cast<ui>(i::OPUSHOBJ)); }

// static Bytecode opushobjprop(Register reg) {
//   return Bytecode(static_cast<ui>(i::OPUSHOBJPROP), reg);
// }

// static Bytecode opusharrobj(Register reg) {
//   return Bytecode(static_cast<ui>(i::OPUSHARROBJ), reg);
// }

// static Bytecode opushkey(Register key) {
//   return Bytecode(static_cast<ui>(i::OPUSHKEY), key);
// }

// static Bytecode opusharr() { return Bytecode(static_cast<ui>(i::OPUSHARR)); }

// static Bytecode opoptarr() { return Bytecode(static_cast<ui>(i::OPOPTARR)); }

// static Bytecode opopts() { return Bytecode(static_cast<ui>(i::OPOPTS)); }

// static Bytecode opoptobj(Register key) {
//   return Bytecode(static_cast<ui>(i::OPOPTOBJ), key);
// }

// static Bytecode oassign(Register key) {
//   return Bytecode(static_cast<ui>(i::OASSIGN), key);
// }

// static Bytecode popa() { return Bytecode(static_cast<ui>(i::POPA)); }

// #define FUNC(name, X)                                                          \
//   \
// static Bytecode                                                                \
//   name() {                                                                     \
//     return Bytecode(static_cast<ui>(i::X));                                    \
//   \
// }

// FUNC(add, ADD)
// FUNC(sub, SUB)
// FUNC(mul, MUL)
// FUNC(div, DIV)
// FUNC(mod, MOD)
// FUNC(shr, SHR)
// FUNC(shl, SHL)
// FUNC(equal, EQ)
// FUNC(_not_eq, NEQ)
// FUNC(lt, LT)
// FUNC(gt, GT)
// FUNC(inc, INC)
// FUNC(dec, DEC)
// FUNC(pinc, PINC)
// FUNC(pdec, PDEC)
// FUNC(neg, NEG)
// FUNC(_or, OR)
// FUNC(_and, AND)
// FUNC(_bitor, BITOR)
// FUNC(_bitand, BITAND)
// FUNC(_bitnot, BITNOT)
// FUNC(_bitxor, BITXOR)

// #undef FUNC

// static Bytecode jmp(Register j) {
//   return Bytecode(static_cast<ui>(i::JMP), Register(j));
// }

// static Bytecode jmpz() { return Bytecode(static_cast<ui>(i::JMPZ)); }

// // call bytecode has following format
// // it contains the information for the number of arguments,
// //
// static Bytecode call(Register reg) {
//   return Bytecode(static_cast<ui>(i::CALL), reg);
// }

// static Bytecode call_native(Register reg) {
//   return Bytecode(static_cast<ui>(i::CALLNATIVE), reg);
// }

// static Bytecode ret() { return Bytecode(i::RET); }

// static Bytecode ret0() { return Bytecode(i::RET0); }

// static Bytecode empty() { return Bytecode(i::EMPTY); }

// static Bytecode hlt() { return Bytecode(i::HLT); }
// }

// #undef I
// #endif // BYTECODE_H_

/**
 * instructions for virtual machine, as previously i used so many different
 * instructions. They were so many that some of them I didn't even use
 * So this time I thought of making simple and multipurpose instructions
 * Last time I cared about the speed of the interpreter. Now I don't care
 * if the speed of te interpreter is slow. All I want is to make it bug free
 *
 * Since everything in this language is object. So these instructions should 
 * focus more on objects. Loading storing should involve objects Not basic types
 * Hence it will be easier to write instructions. Since there would be
 * uniformity.
 *
 * About virtual machine
 * ========================
 * Virtual machine will be based on Stack oriented. That is, instructions like 
 *
 *          add a, b
 *
 * will be transformed as 
 *
 *          push a
 *          push b
 *          add
 *
 * This will be basis for almost every instruction. Also since we have to store 
 * the variable in the memory. But since the language is dynamic, the variable
 * must be stored in a way so that we can easily access that variable at later
 * time. That is basis for almost all dynamic languages. Example javascript,
 * python etc. 
 *
 * Now to store the variable (so that we can later access it by its name), we've
 * to store it with some relation to its name. One way is to store all the variable
 * data in contigous memory. In this way we can access the variable in O(1) time,
 * but we've to store the offset of the data in some relation to its name. 
 * That we can do by mapping the addresses with names in map<string, int>. So
 * we have to find the address corresponding to the name at the parsing time. 
 * At the runtime we will get performance boost. But code generation in this
 * case will be difficult for me. Another way which I prefer is to store the
 * variable directly in map without involvement of any offsets i.e. 
 *
 *            map<string, object>
 * 
 * Here, we can access a variable in O(log n) time. That will impact performance.
 * 
 *
 * Instructions
 * -------------------
 *
 *  + Immediate Instructions ==> (LD 0), (LD "prince")
 *  + Direct Address Instructions ==> (LD a), (LD b)
 *  + Allocation Deallocation Instructions ==> (NEW a) (NEW a 0) (NEW a "string")
 *  + Indexed instructions ==> (LDI 0) (LDI "str")  ## `a` is in accumulator
 *  + Arithematic instructions ==> (ADD b) (ADD 0) (ADD a[0]) (ADD a['str'])
 *
 */

#ifndef BYTECODE_H_
#define BYTECODE_H_

#include "object.h"
#include <vector>

#define BYTECODE_LIST(L)    \
 L(ld),  \
 L(nw),  \
 L(dl),  \
 L(li),  \
 L(ad),  \
 L(sb),  \
 L(ml),  \
 L(dv),  \
 L(md),  \
 L(cl),  \
 L(rt)

enum class BCode {
#define ENUM(op)    op
BYTECODE_LIST(ENUM)
#undef ENUM
};

const char *bcode_str[] = {
#define STR(op) #op
BYTECODE_LIST(STR)
#undef STR
};

#undef BYTECODE_LIST
#define BCODE_TYPE_LIST(L)  \
 L(imm),    \
 L(dir),    \
 L(idx),    \
 L(alc)

enum class BType {
#define ENUM(type) type
BCODE_TYPE_LIST(ENUM)
#undef ENUM
};

const char *btype_str[] = {
#define STR(type) #type
BCODE_TYPE_LIST(STR)
#undef STR
};

#undef BCODE_TYPE_LIST

class Bytecode {
public:
    Bytecode() = default;
    Bytecode(const Bytecode &) = default;
    Bytecode &operator=(const Bytecode &) = default;
    ~Bytecode() = default;

    void set(BCode code, BType type, Object data)
    {
        code_ = code;
        type_ = type;
        data_ = data;
    }

private:
    BCode code_;
    BType type_;
    Object data_;
};

using BytecodeList = std::vector<Bytecode>;

#endif

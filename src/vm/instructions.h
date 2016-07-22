#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "vm/instruction.h"

namespace grok {
namespace vm {

using InstructionKind = enum Instructions;

#define DEFINE_INSTRUCTION(instr_class, instr_kind) \
    InstructionKind GetKind() const override    \
    {   \
        return InstructionKind::instr_kind; \
    }   \
    void AcceptVisitor(InstructionVisitor *visitor) override;

class InstructionVisitor;

// basic class for instruction
class NoopInstruction {
public:
    virtual InstructionKind GetKind() const
    {
        return InstructionKind::noop;
    }

    static NoopInstruction *Create()
    {
        return new NoopInstruction();
    }

    virtual void AcceptVisitor(InstructionVisitor *visitor);

    virtual ~NoopInstruction() { }
};

// fetch instruction ::= fetches the operand from the VStore
// This instruction uses `name` (which is a string) to refer to operand,
// This is most likely to be changed due to performance reasons
class FetchInstruction : public NoopInstruction {
private:
    std::string name_;
    size_t addr_;
public:
    FetchInstruction(const std::string &name)
        : name_ { name }
    { }

    FetchInstruction(size_t addr)
        : addr_{ addr }
    { }

    const std::string &name() const { return name_; }

    DEFINE_INSTRUCTION(FetchInstruction, fetch)

    static FetchInstruction *Create(const std::string &name)
    {
        return new FetchInstruction(name);
    }

    static FetchInstruction *Create(size_t addr)
    {
        return new FetchInstruction(addr);
    }
};

// store instruction pops the value from the top and stores that
// value to variable referred by name_
class StoreInstruction : public NoopInstruction {
private:
    std::string name_;
    size_t addr_;
public:
    StoreInstruction(const std::string &name)
        : name_ { name }
    { }

    StoreInstruction(size_t addr)
        : addr_{ addr }
    { }

    const std::string &name() const { return name_; }

    DEFINE_INSTRUCTION(StoreInstruction, store)

    static StoreInstruction *Create(const std::string &name)
    {
        return new StoreInstruction(name);
    }

    static StoreInstruction *Create(size_t addr)
    {
        return new StoreInstruction(addr);
    }
};

// markst ::= marks the state before a constructor call
// a temporary hack to get constructors work
class MarkstInstruction : public NoopInstruction {
public:
    DEFINE_INSTRUCTION(MarkstInstruction, markst)

    static MarkstInstruction *Create()
    {
        return new MarkstInstruction();
    }
};

class PushInstruction : public NoopInstruction {
public:
    PushInstruction(grok::obj::Handle* obj)
        : obj_{ obj }
    { }

    grok::obj::Handle *handle() { return obj_; }

    DEFINE_INSTRUCTION(PushInstruction, push)

    static PushInstruction *Create(grok::obj::Handle *obj)
    {
        return new PushInstruction(obj);
    }
private:
    grok::obj::Handle* obj_;
};

class CountInstruction : public NoopInstruction {
private:
    size_t count_;
public:
    CountInstruction(size_t addr)
        : count_{ addr }
    { }

    InstructionKind GetKind() const override
    {
        return InstructionKind::noop;
    }

    size_t count() { return count_; }
};

class PopPropInstruction : public CountInstruction {
public:
    PopPropInstruction(size_t addr)
        : CountInstruction(addr)
    { }

    DEFINE_INSTRUCTION(PopPropInstruction, poprop)

    static PopPropInstruction *Create(size_t addr)
    {
        return new PopPropInstruction(addr);
    }
};

class IndexInstruction : public CountInstruction {
public:
    IndexInstruction(size_t addr)
        : CountInstruction(addr)
    { }

    DEFINE_INSTRUCTION(IndexInstruction, index)

    static IndexInstruction *Create(size_t addr)
    {
        return new IndexInstruction(addr);
    }
};

class ResInstruction : public CountInstruction {
public:
    ResInstruction(size_t addr)
        : CountInstruction(addr)
    { }

    DEFINE_INSTRUCTION(ResInstruction, res)

    static ResInstruction *Create(size_t addr)
    {
        return new ResInstruction(addr);
    }
};

class NewsInstruction : public NoopInstruction {
private:
    std::string name_;
public:
    NewsInstruction(const std::string &name)
        : name_{ name }
    { }

    DEFINE_INSTRUCTION(NewsInstruction, news)

    const std::string& name() const { return name_; }

    static NewsInstruction *Create(const std::string &name)
    {
        return new NewsInstruction(name);
    }
};

class CopyAInstruction : public CountInstruction {
public:
    CopyAInstruction(size_t count)
        : CountInstruction(count)
    { }

    DEFINE_INSTRUCTION(CopyAInstruction, cpya)

    static CopyAInstruction *Create(size_t count)
    {
        return new CopyAInstruction(count);
    }
};

class ReferenceInstruction : public NoopInstruction {
private:
    std::string name_;
public:
    ReferenceInstruction(const std::string &str)
        : name_{ str }
    { }

    const std::string& name() const { return name_; }
};

class MapsInstruction : public ReferenceInstruction {
public:
    MapsInstruction(const std::string &str)
        : ReferenceInstruction(str)
    { }

    DEFINE_INSTRUCTION(MapsInstruction, maps)

    static MapsInstruction *Create(const std::string &str)
    {
        return new MapsInstruction(str);
    }
};

class LoopzInstruction : public CountInstruction {
public:
    LoopzInstruction(size_t addr)
        : CountInstruction(addr)
    { }

    DEFINE_INSTRUCTION(LoopzInstruction, loopz)

    static LoopzInstruction *Create(size_t addr)
    {
        return new LoopzInstruction(addr);
    }
};

class JmpInstruction : public CountInstruction {
public:
    JmpInstruction(size_t addr)
        : CountInstruction(addr)
    { }

    DEFINE_INSTRUCTION(JmpInstruction, jmp)

    static JmpInstruction *Create(size_t addr)
    {
        return new JmpInstruction(addr);
    }
};

class CallInstruction : public CountInstruction {
public:
    CallInstruction(size_t count)
        : CountInstruction(count)
    { }

    DEFINE_INSTRUCTION(CallInstruction, call)

    static CallInstruction *Create(size_t count)
    {
        return new CallInstruction(count);
    }
};

class JmpzInstruction : public CountInstruction {
public:
    JmpzInstruction(size_t count)
        : CountInstruction(count)
    { }

    DEFINE_INSTRUCTION(JmpzInstruction, jmpz)

    static JmpzInstruction *Create(size_t count)
    {
        return new JmpzInstruction(count);
    }
};

class JmpnzInstruction : public CountInstruction {
public:
    JmpnzInstruction(size_t count)
        : CountInstruction(count)
    { }

    DEFINE_INSTRUCTION(JmpnzInstruction, jmpnz)

    static JmpnzInstruction *Create(size_t count)
    {
        return new JmpnzInstruction(count);
    }
};

class ReplPropInstruction : public NoopInstruction {
private:
    std::string name_;
public:
    ReplPropInstruction(const std::string &str)
        : name_{ str }
    { }

    DEFINE_INSTRUCTION(ReplPropInstruction, replprop)

    const std::string& name() const { return name_; }

    static ReplPropInstruction *Create(const std::string &str)
    {
        return new ReplPropInstruction(str);
    }
};


#define ZERO_OPERAND_INSTRUCTION(name, kind)    \
class name : public NoopInstruction {  \
public:  \
    DEFINE_INSTRUCTION(name, kind)  \
    \
    static name *Create()   \
    { \
        return new name();  \
    }   \
}

ZERO_OPERAND_INSTRUCTION(PopInstruction, pop);
ZERO_OPERAND_INSTRUCTION(PushimInstruction, pushim);
ZERO_OPERAND_INSTRUCTION(PushThisInstruction, pushthis);
ZERO_OPERAND_INSTRUCTION(IncInstruction, inc);
ZERO_OPERAND_INSTRUCTION(DecInstruction, dec);
ZERO_OPERAND_INSTRUCTION(BNotInstruction, bnot);
ZERO_OPERAND_INSTRUCTION(SNotInstruction, snot);
ZERO_OPERAND_INSTRUCTION(PincInstruction, pinc);
ZERO_OPERAND_INSTRUCTION(PdecInstruction, pdec);
ZERO_OPERAND_INSTRUCTION(LtsInstruction, lts);
ZERO_OPERAND_INSTRUCTION(GtsInstruction, gts);
ZERO_OPERAND_INSTRUCTION(LtesInstruction, ltes);
ZERO_OPERAND_INSTRUCTION(GtesInstruction, gtes);
ZERO_OPERAND_INSTRUCTION(EqsInstruction, eqs);
ZERO_OPERAND_INSTRUCTION(NEqsInstruction, neqs);
ZERO_OPERAND_INSTRUCTION(AddsInstruction, adds);
ZERO_OPERAND_INSTRUCTION(SubsInstruction, subs);
ZERO_OPERAND_INSTRUCTION(MulsInstruction, muls);
ZERO_OPERAND_INSTRUCTION(DivsInstruction, divs);
ZERO_OPERAND_INSTRUCTION(ShlsInstruction, shls);
ZERO_OPERAND_INSTRUCTION(ShrsInstruction, shrs);
ZERO_OPERAND_INSTRUCTION(RemsInstruction, rems);
ZERO_OPERAND_INSTRUCTION(BorsInstruction, bors);
ZERO_OPERAND_INSTRUCTION(BandsInstruction, bands);
ZERO_OPERAND_INSTRUCTION(OrsInstruction, ors);
ZERO_OPERAND_INSTRUCTION(AndsInstruction, ands);
ZERO_OPERAND_INSTRUCTION(XorsInstruction, xors);
ZERO_OPERAND_INSTRUCTION(MemCallInstruction, mem_call);
ZERO_OPERAND_INSTRUCTION(LeaveInstruction, leave);
ZERO_OPERAND_INSTRUCTION(RetInstruction, ret);

#undef ZERO_OPERAND_INSTRUCTION

}
}
#endif

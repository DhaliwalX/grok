#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include "object/jsbasicobject.h"
#include <vector>

namespace grok {
namespace obj {

class Argument : public JSObject {
public:
    using Pointer = std::shared_ptr<Object>;
    using size_type = std::vector<Pointer>::size_type;
    using iterator = std::vector<Pointer>::iterator;

    Argument() {}

    Argument(std::vector<Pointer> args)
        : args_{ std::move(args) }
    { }

    Pointer operator[](size_type i)
    {
        return args_[i];
    }

    Pointer At(size_type i)
    {
        if (i > Size()) {
            return this->GetProperty(std::to_string(i));
        }
        return args_[i];
    }

    Pointer At(const std::string &prop)
    {
        size_type idx = 0;
        // we still try to convert the string into number
        try {
            idx = std::stod(prop);
        } catch (...) {
            return this->GetProperty(prop);
        }
        return this->At(idx);
    }

    ObjectType GetType() const override
    {
        return ObjectType::_argument;
    }

    bool Empty() const
    {
        return args_.empty();
    }

    size_type Size() const
    {
        return args_.size();
    }

    void Resize(size_type s)
    {
        args_.resize(s, CreateUndefinedObject());
    }

    void Assign(size_type idx, Pointer obj)
    {
        if (idx > Size()) {
            this->AddProperty(std::to_string(idx), obj);
            return;
        }
        args_[idx] = obj;
    }

    const std::vector<Pointer>& Container() const
    {
        return args_;
    }

    void Push(const Pointer &obj)
    {
        args_.push_back(obj);
    }

    Pointer Pop()
    {
        auto ret = args_.back();
        args_.pop_back();
        return ret;
    }

    iterator begin()
    {
        return args_.begin();
    }

    iterator end()
    {
        return args_.end();
    }
private:
    std::vector<Pointer> args_;
};

static inline auto CreateArgumentObject()
{
    auto arg = std::make_shared<Argument>();
    auto O = std::make_shared<Object>(arg);
    return O;
}

} // obj
} // grok

#endif // argument.h

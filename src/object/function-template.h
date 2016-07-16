#ifndef FUNCTION_TEMPLATE_H_
#define FUNCTION_TEMPLATE_H_

#include "object/function.h"

namespace grok {
namespace obj {

template <typename Callable>
class FunctionTemplate : public Function {
public:
    FunctionTemplate(Callable func)
        : Function{ }, func_{ func }
    {
        this->Native = true;
    }

    grok::vm::Value CallNative(std::vector<grok::vm::Value> &Args,
        std::shared_ptr<Handle> This) override
    {
        auto Obj = CreateArgumentObject();
        auto ArgObj = Obj->as<Argument>();
        size_t idx = 0, max = 0;
        auto vec = Params;

        max = vec.size();
        for (auto i : Args) {
            ArgObj->Push(i.O);

            // we don't want to lose extra args
            if (idx < max)
                ArgObj->AddProperty(vec[idx++], i.O);
        }

        while (idx < max) {
            ArgObj->AddProperty(vec[idx++], CreateUndefinedObject());
        }

        auto ThisWrapped = std::make_shared<Object>(This);
        ArgObj->AddProperty(std::string("this"), ThisWrapped);
        // call the native function
        auto ret = func_(ArgObj);

        auto wrapped = Value(ret);
        return wrapped;
    }

private:
    Callable func_;
};

template <class Callable> 
static std::shared_ptr<Object>
CreateFunction(Callable func)
{
    auto F = std::make_shared<FunctionTemplate<Callable>>(func);
    DefineInternalObjectProperties(F.get());
    return std::make_shared<Object>(F);
}

}
}

#endif

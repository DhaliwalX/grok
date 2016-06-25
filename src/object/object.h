#ifndef REGISTER_H_
#define REGISTER_H_

#include <memory>

namespace grok { namespace obj {
class Object {
public:
    Object() :
        data_ { }
    { }

    template <typename T>
    Object(T *data)
        : data_(data)
    { }

    template <typename T>
    Object(std::shared_ptr<T> ptr)
        : data_(std::static_pointer_cast<void>(ptr))
    { }

    template <typename T>
    Object(std::unique_ptr<T> ptr)
        : data_(std::static_pointer_cast<void>(std::move(ptr)))
    { }

    Object(const Object &reg)
        : data_(reg.data_)
    { }

    Object(Object &&reg)
        : data_(std::move(reg.data_))
    { }

    Object &operator=(const Object &reg)
    {
        data_ = reg.data_;
        return *this;
    }

    template <typename C>
    auto as() -> decltype(std::shared_ptr<C>())
    {
        if (empty()) {
            throw std::runtime_error("invalid pointer access");
        }
        return std::static_pointer_cast<C>(data_);
    }

    bool empty() const {
        return !data_;
    }

private:
    std::shared_ptr<void> data_;
};

} // obj
} // grok
#endif


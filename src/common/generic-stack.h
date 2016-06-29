#ifndef GENERIC_STACK_H_
#define GENERIC_STACK_H_

#include <vector>
#include <stdexcept>

/// Stack ::= used for running interpreter for storing objects
template <typename T>
class GenericStack : public std::vector<T> {
public:
    using base = std::vector<T>;

    GenericStack()
        : std::vector<T>{}
    { }

    void Push(T object)
    {
        base::push_back(object);
    }
    
    T Pop()
    {
        if (Empty()) {
            throw std::runtime_error("fatal: stack was empty");
        }
        auto V = base::back();
        base::pop_back();
        return V;
    }

    T& Top()
    {
        if (Empty()) {
            throw std::runtime_error("fatal: stack was empty");
        }
        return base::back();
    }

    bool Empty()
    {
        return base::empty();
    }
};

#endif

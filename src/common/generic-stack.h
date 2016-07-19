#ifndef GENERIC_STACK_H_
#define GENERIC_STACK_H_

#include <vector>
#include <stdexcept>

/// Stack ::= used for running interpreter for storing objects
template <typename T>
class GenericStack {
public:
    using base = std::vector<T>;
    using size_type = typename base::size_type;

    GenericStack()
        : container_{}
    { }

    void Push(T object)
    {
        container_.push_back(object);
    }
    
    T Pop()
    {
        if (Empty()) {
            throw std::runtime_error("fatal: stack was empty");
        }
        auto V = container_.back();
        container_.pop_back();
        return V;
    }

    T& Top()
    {
        if (Empty()) {
            throw std::runtime_error("fatal: stack was empty");
        }
        return container_.back();
    }

    bool Empty() const
    {
        return container_.empty();
    }

    ~GenericStack()
    {
    }

    T &operator[](size_t idx)
    {
        return container_[idx];
    }

    void push_back(const T &t)
    {
        container_.push_back(t);
    }

    typename base::reverse_iterator rbegin()
    {
        return container_.rbegin();
    }

    typename base::reverse_iterator rend()
    {
        return container_.rend();
    }


    typename base::iterator begin()
    {
        return container_.begin();
    }


    typename base::iterator end()
    {
        return container_.end();
    }

    void clear()
    {
        container_.clear();
    }

    bool empty() const 
    {
        return Empty();
    }

    size_type size() const
    {
        return container_.size();
    }

    void resize(size_type sz)
    {
        container_.resize(sz);
    }
private:
    base container_;
};

#endif

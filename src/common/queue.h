#ifndef QUEUE_H_
#define QUEUE_H_

namespace grok {

template <class T, class Container>
class Queue {
public:
    Queue() :
        q_{ }
    { }

    T &Front()
    {
        return q_.front();
    }

    T PopFront()
    {
        auto ret = q_.front();
        q_.pop_back();
        return ret;
    }

    void Push(const T &e)
    {
        q_.push_back(e);
    }

    Container &Cont()
    {
        return q_;
    }

    bool Empty() const
    {
        return q_.empty();
    }

    typename Container::size_type Size() const
    {
        return q_.size();
    }
private:
    Container q_;
};

}

#endif

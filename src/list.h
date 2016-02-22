#ifndef LIST_H_
#define LIST_H_

#include <memory>
#include <exception>
#include <vector>
#include <functional>

namespace list {

// doubly linked list node
template <typename DataType> struct list_node {
  DataType data_;
  list_node *next_;
  list_node *prev_;

  // default constructor
  list_node() : data_() {}

  list_node(const DataType &data)
      : data_(data), next_(nullptr), prev_(nullptr) {}

  list_node(const list_node &rhs)
      : data_(rhs.data_), next_(rhs.next_), prev_(rhs.prev_) {}

  list_node &operator=(const list_node &rhs) {
    data_ = rhs.data_;
    next_ = rhs.next_;
    prev_ = rhs.prev_;
    return *this;
  }

  void remove_links() { next_ = prev_ = nullptr; }

  ~list_node() = default;
};

// list_iterator : iterator class for doubly linked list node
template <typename DataType> class list_iterator {
public:
  using size_type = unsigned long long;
  using reference = DataType;

  // default constructor
  list_iterator() : curr_{nullptr} {}

  // from a pointer
  list_iterator(list_node<DataType> *curr) : curr_(curr) {}

  list_iterator(const list_iterator &rhs) : curr_(rhs.curr_) {}

  list_iterator &operator=(const list_iterator &rhs) {
    curr_ = rhs.curr_;
    return *this;
  }

  // pre increment operator : go to next node.
  // It stops incrementing after reaching end
  list_iterator operator++() {
    if (curr_) {
      if (curr_->next_)
        return list_iterator(curr_ = curr_->next_);
      else //  we're at end so we don't want to increment here
        return *this;
    }
    throw std::out_of_range("pointer was NULL");
  }

  // post-increment operator
  list_iterator operator++(int) {
    if (curr_) {
      if (curr_->next_) {
        list_node<DataType> *save = curr_;
        curr_ = curr_->next_;
        return list_iterator(save);
      } else
        return *this;
    }
    throw std::out_of_range("pointer was NULL");
  }

  // predecrement operator : go back to previous node
  list_iterator operator--() {
    if (curr_) {
      if (curr_->prev_)
        return list_iterator(curr_ = curr_->prev_);
      else //  we're at start and we don't want to decrement here
        return *this;
    }
    throw std::out_of_range("pointer was NULL");
  }

  // post-decrement operator
  list_iterator operator--(int) {
    if (curr_) {
      if (curr_->next_) {
        list_node<DataType> *save = curr_;
        curr_ = curr_->prev_;
        return list_iterator<DataType>(save);
      } else
        return *this;
    }
    throw std::out_of_range("pointer was NULL");
  }

  // plus operator
  list_iterator<DataType> operator+(size_type off) {
    list_iterator<DataType> node = *this;
    while (off--)
      ++node;
    return node;
  }

  // minus operator
  list_iterator operator-(size_type off) {
    list_iterator<DataType> node = *this;
    while (off--)
      --node;
    return node;
  }

  // dereference operator
  DataType &operator*() { return curr_->data_; }

  // arrow operator
  DataType *operator->() { return &(curr_->data_); }

  bool operator==(const list_iterator &rhs) { return curr_ == rhs.curr_; }

  bool operator!=(const list_iterator &rhs) { return !(*this == rhs); }

  list_node<DataType> *_Ptr() const { return curr_; }

  list_node<DataType> *&_Ptr() { return curr_; }

private:
  list_node<DataType> *curr_;
};

// main list class
template <typename DataType> class List {
public:
  typedef typename list_iterator<DataType> iterator;
  typedef typename list_iterator<DataType>::size_type size_type;

  List()
      : head_(new list_node<DataType>()), last_(new list_node<DataType>()),
        size_(0) {
    init();
  }

  List(const std::initializer_list<DataType> &list)
      : head_(new list_node<DataType>()), last_(new list_node<DataType>()),
        size_(0) {
    init();
    for (const auto &element : list) {
      _push_back(new list_node<DataType>(element));
    }
  }

  ~List() {
    erase(begin(), end());
    delete head_;
    delete last_;
  }

  // insert a range of element to the list after a given position
  // \complexity : O(n)
  iterator insert(const iterator &pos, const iterator &first,
                  const iterator &last) {
    iterator pos_c = pos;
    for (iterator it = first; it != last; ++it) {
      pos_c = insert(pos_c, *it);
    }
    return pos_c;
  }

  // get an element at a given index
  // \complexity : O(n)
  DataType &at(int off) {
    if (off <= this->size_)
      return *(begin() + off);
    throw std::out_of_range("");
  }

  // remove an element pointed by iterator
  // \complexity : O(1)
  iterator erase(iterator &element) {
    auto ret = element + 1;
    unlink(element._Ptr());
    delete element._Ptr();
    return ret;
  }

  // erase a range of elements [first, second)
  // \complexity : O(n)
  iterator erase(iterator &first, iterator &second) {
    auto it = first;
    while (it != second) {
      it = erase(it);
    }
    return it;
  }

  // insert an element after a given position
  // \complexity : O(1)
  iterator insert(const iterator &pos, const DataType &data) {
    list_node<DataType> *data_node{new list_node<DataType>(data)};
    _insert_after(pos._Ptr(), data_node);
    return iterator(data_node);
  }

  // inserts an element at the end
  void push_back(const DataType &data) {
    _push_back(new list_node<DataType>(data));
  }

  // get iterator to the beginning of the list
  iterator begin() { return iterator(head_->next_); }

  // get iterator to the end of the list
  iterator end() { return iterator(last_); }

  bool empty() const { return !size_; }

  // removes the last element
  inline void pop_back() { erase(--end()); }

  DataType &back() { return *iterator(end() - 1); }

  DataType back() const { return *iterator(end() - 1); }

  void clear() { erase(begin(), end()); }

  size_type size() const { return this->size_; }

  // rotates the list towards left with first element to the last
  // rotation occurs in constant time
  void rotate_left() {
    if (size() >
        size_type(1)) { // rotation occurs only if size is greater than 1
      auto save = head_->next_;
      head_->next_ = save->next_;
      head_->next_->prev_ = head_;
      last_->prev_->next_ = save;
      save->prev_ = last_->prev_;
      last_->prev_ = save;
      save->next_ = last_;
    }
  }

  void rotate_right() {
    if (size() > size_type(1)) {
      auto save = last_->prev_;
      last_->prev_ = save->prev_;
      last_->prev_->next_ = last_;
      head_->next_->prev_ = save;
      save->next_ = head_->next_;
      save->prev_ = head_;
      head_->next_ = save;
    }
  }

  // list will be invalidated after this function call
  void append(List<DataType> &list) {
    this->last_->prev_->next_ = list.head_->next_;
    list.head_->next_->prev_ = this->last_->prev_;
    this->last_ = list.last_;
    this->size_ += list.size();
    list.init();
  }

private:
  // internal routine of push_back
  inline void _push_back(list_node<DataType> *element) {
    element->next_ = last_;
    element->prev_ = last_->prev_;
    last_->prev_->next_ = element;
    last_->prev_ = element;
    size_++;
  }

  inline void _insert_after(list_node<DataType> *pos,
                            list_node<DataType> *element) {
    if (pos == last_)
      return _insert_before(pos, element);
    element->next_ = pos->next_;
    element->prev_ = pos;
    pos->next_->prev_ = element;
    pos->next_ = element;
    size_++;
  }

  inline void _insert_before(list_node<DataType> *pos,
                             list_node<DataType> *element) {
    element->next_ = pos;
    element->prev_ = pos->prev_;
    pos->prev_->next_ = element;
    pos->prev_ = element;
    size_++;
  }

  inline void unlink(list_node<DataType> *element) {
    element->prev_->next_ = element->next_;
    element->next_->prev_ = element->prev_;
    size_--;
  }

  // initialize doubly linked list
  inline void init() {
    head_->next_ = last_;
    head_->prev_ = nullptr;
    last_->prev_ = head_;
    last_->next_ = nullptr;
    size_ = 0;
  }

  list_node<DataType> *head_;
  list_node<DataType> *last_;
  size_type size_;
};

// node class for singly linked list
template <typename DataType> class forward_list_node {
public:
  forward_list_node(const DataType &data) : data_(data), next_(nullptr) {}

  forward_list_node() : data_(), next_(nullptr) {}

  forward_list_node(const forward_list_node &rhs)
      : data_(rhs.data_), next_(rhs.next_) {}

  forward_list_node &operator=(const forward_list_node &rhs) {
    data_ = rhs.data_;
    next_ = rhs.next_;
  }

  ~forward_list_node() = default;

  DataType data_;
  forward_list_node<DataType> *next_;
};

// iterator class for singly linked list
template <typename DataType> class f_list_iterator {
public:
  using NodePtr = typename forward_list_node<DataType> *;
  using reference = DataType;
  using pointer = DataType *;
  using ptrdiff_t = std::ptrdiff_t;

  f_list_iterator() : current_(nullptr) {}

  f_list_iterator(NodePtr ptr) : current_(ptr) {}

  f_list_iterator(const f_list_iterator &rhs) : current_(rhs.current_) {}

  f_list_iterator &operator=(const f_list_iterator &rhs) {
    current_ = rhs.current_;
    return *this;
  }

  ~f_list_iterator() = default;

  reference &operator*() { return current_->data_; }

  pointer operator->() { return (&(current_->data_)); }

  f_list_iterator &operator++() {
    if (current_) {
      if (current_->next_) {
        // we are not at end yet
        current_ = current_->next_;
      }
      return (*this);
    }
    throw std::out_of_range("pointer was NULL");
  }

  f_list_iterator operator++(int) {
    auto save = (*this);
    ++(*this);
    return save;
  }

  f_list_iterator operator+(ptrdiff_t off) {
    auto save = (*this);
    while (off--) {
      ++save;
    }
    return save;
  }

  bool operator!=(const f_list_iterator &rhs) {
    return rhs.current_ != current_;
  }

  bool operator==(const f_list_iterator &rhs) { return !((*this) != rhs); }

  NodePtr &_Ptr() { return current_; }

private:
  forward_list_node<DataType> *current_;
};

template <typename DataType> class forward_list {
public:
  using iterator = typename f_list_iterator<DataType>;
  using NodePtr = typename iterator::NodePtr;
  using reference = typename iterator::reference;
  using pointer = typename iterator::pointer;
  using Node = typename forward_list_node<DataType>;
  using size_type = typename unsigned long long;

  forward_list() : head_(new Node()), end_(new Node()), size_(0) { init(); }

  forward_list(const std::initializer_list<DataType> &list)
      : head_(new Node()), end_(new Node()),
        size_(0) { // create list from initializer list
    init();
    auto beg = list.begin();
    iterator pos(head_);
    while (beg != list.end()) {
      pos = insert_after(pos, *beg);
      ++beg;
    }
  }

  ~forward_list() {
    erase_after(begin(), end());
    delete head_;
    delete end_;
  }

  iterator begin() { return iterator(head_->next_); }

  iterator end() { return iterator(end_); }

  // add a new element to the end
  // \complexity : O(1)
  // may invalidate iterators pointing to the end
  void push_back(const DataType &data) { insert(end_, new Node(data)); }

  void push_front(const DataType &data) { insert(head_, new Node(data)); }

  void pop_front(const DataType &data) {
    auto rem = head_->next_;
    remove(head_, rem);
    delete rem;
  }

  // remove the last element
  // \complexity : O(n)
  void pop_back() {
    NodePtr e = go_back(2, end_), rem;
    remove(e, rem = e->next_);
    delete rem;
  }

  // erase an element after a given position
  // \complexity : O(1);
  iterator erase_after(iterator element) {
    NodePtr e = element._Ptr()->next_;
    remove(element._Ptr(), e);
    delete e;
    return element;
  }

  // erase a range of elements
  // \complexity : O(n)
  iterator erase_after(iterator start, iterator end) {
    while (start._Ptr()->next_ != end._Ptr()) {
      erase_after(start);
    }
    return start;
  }

  // insert an element after a given position
  // \complexity : O(1)
  iterator insert_after(iterator pos, const DataType &data) {
    insert(pos._Ptr(), new Node(data));
    return ++iterator(pos);
  }

  // insert a range of elements
  // \complexity : O(n)
  template <typename AnotherIterator>
  iterator insert_after(iterator pos, AnotherIterator start,
                        AnotherIterator end) {
    while (start != end) {
      pos = insert_after(pos, (*start));
      ++start;
    }
    return pos;
  }

  size_type size() const { return this->size_; }
  bool empty() const { return !this->size_; }
  NodePtr &GetHead() { return head_; }
  NodePtr &GetEnd() { return end_; }
  // remove all the elements from the list
  void clear() { erase_after(begin(), end()); }

  // find a particular element and return an iterator to it
  // \complexity : O(n)
  iterator find(const DataType &data) {
    iterator beg = begin();
    for (; beg != end(); ++beg)
      if (*beg == data)
        break;
    return beg;
  }

protected:
  virtual void insert(NodePtr pos, NodePtr element) {
    if (pos == end_) {
      // if we're at end then, we just copy the data to the end_, and
      // make the element new end_, by resetting the data_. It thus
      // invalidates the iterator pointing to the end
      end_->data_ = element->data_;
      end_->next_ = element;
      end_ = element;
      element->data_ = DataType();
      size_++;
      return;
    }
    element->next_ = pos->next_;
    pos->next_ = element;
    size_++;
  }

  virtual void remove(NodePtr &prev, NodePtr &element) {
    prev->next_ = element->next_;
    size_--;
  }

  // initialize the list
  inline void init() {
    head_->next_ = end_;
    end_->next_ = nullptr;
  }

  // go back from a position in the singly linked list by off
  // this function is quite safe. You can never go back the head_
  // \complexity : O(n)
  NodePtr go_back(ptrdiff_t off, NodePtr pos) {
    iterator head{head_};
    iterator save = head;
    head = head + off;
    while (head._Ptr() != pos) {
      ++save;
      ++head;
    }
    return save._Ptr();
  }

  size_type size_;
  NodePtr head_;
  NodePtr end_;
};

// always sorted list
template <typename DataType>
class sorted_list : private ::list::forward_list<DataType> {
public:
  using iterator = typename list::f_list_iterator<DataType>;
  using NodePtr = typename iterator::NodePtr;
  using reference = typename iterator::reference;
  using pointer = typename iterator::pointer;
  using Node = typename list::forward_list_node<DataType>;
  using size_type = typename unsigned long long;

  using base = list::forward_list<DataType>;
  using predicate_type =
      typename std::function<bool(const DataType &, const DataType &)>;

  // inherit some base methods to public part
  using base::size;
  using base::empty;
  using base::begin;
  using base::end;
  using base::clear;
  using base::erase_after;

  // default constructor
  sorted_list() : base(), pred_(std::less<DataType>()) {}

  // constructor using predicate
  explicit sorted_list(predicate_type &pred) : base(), pred_(pred) {}

  sorted_list(const std::initializer_list<DataType> &list)
      : base(list), pred_(std::less<DataType>()) {}

  // insert the element into the list at  correct position
  // \complexity : O(n)
  void insert(const DataType &data) {
    auto pos = find_position(data);
    base::insert(pos, new Node(data));
  }

private:
  // returns the pointer to the largest node such that data > node->data_
  // \complexity : O(n)
  NodePtr find_position(const DataType &data) {
    iterator beg = begin();
    iterator save = beg, en = end();
    while (pred_(*beg, data)) {
      save = beg;
      ++beg; // this method is safer
      if (beg == en)
        break;
    }

    // in case the element that is to be added is smaller than first element
    // then, we have to add the element just after the head_ node. Since
    // iterator can never point to head_ node so head_ node is explicitly
    // returned
    return save == beg ? head_ : save._Ptr();
  }

  // some better method is required than this
  predicate_type pred_;
};

} // namespace list
#endif // FORWARD_LIST_H_
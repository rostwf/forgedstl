#ifndef FORGED_STL_INTERNAL_QUEUE_H_
#define FORGED_STL_INTERNAL_QUEUE_H_

#include "stl_deque.h"
#include "stl_heap.h"
#include "stl_vector.h"

namespace forgedstl {

template <typename T, typename Sequence = deque<T> >
class queue {
    friend bool operator== <> (const queue&, const queue&);
    friend bool operator< <> (const queue&, const queue&);

public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    reference front() {
        return c.front();
    }
    const_reference front() const {
        return c.front();
    }

    reference back() {
        return c.back();
    }
    const_reference back() const {
        return c.back();
    }

    void push(const value_type& x) {
        c.push_back(x);
    }

    void pop() {
        c.pop_front();
    }

protected:
    Sequence c;
};

template <typename T, typename Sequence>
bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.c == y.c;
}

template <typename T, typename Sequence>
bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.c < y.c;
}

template <typename T, typename Sequence = vector<T>,
          typename Compare = std::less<typename Sequence::value_type> >
class priority_queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

    priority_queue() : c() { }
    explicit priority_queue(const Compare& x) : c(), comp(x) { }
    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x)
        : c(first, last), comp(x) {
        forgedstl::make_heap(c.begin(), c.end(), comp);
    }
    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last)
        : c(first, last) {
        forgedstl::make_heap(c.begin(), c.end(), comp);
    }

    bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    const_reference top() const {
        return c.front();
    }
    void push(const value_type& x) {
        try {
            c.push_back(x);
            forgedstl::push_heap(c.begin(), c.end(), comp);
        } catch (...) {
            c.clear();
            throw;
        }
    }
    void pop() {
        try {
            forgedstl::pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        } catch (...) {
            c.clear();
            throw;
        }
    }

protected:
    Sequence c;
    Compare comp;
};

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_QUEUE_H_

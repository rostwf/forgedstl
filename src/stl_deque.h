#ifndef FORGED_STL_INTERNAL_DEQUE_H_
#define FORGED_STL_INTERNAL_DEQUE_H_

#include <algorithm>

#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_iterator.h"
#include "stl_uninitialized.h"

namespace forgedstl {

// n: buffer size, sz: sizeof(value_type)
inline size_t __deque_buf_size(size_t n, size_t sz) {
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <typename T, typename Ref, typename Ptr, size_t BufSize>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufSize> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSize> const_iterator;

    typedef std::random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;

    typedef __deque_iterator self;

    T* cur;
    T* first;
    T* last;
    map_pointer node;

    static size_t buffer_size() {
        return __deque_buf_size(BufSize, sizeof(T));
    }

    __deque_iterator(T* x, map_pointer y)
        : cur(x), first(*y), last(*y + buffer_size()), node(y) { }
    __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr),
        node(nullptr) { }
    __deque_iterator(const iterator& x)
        : cur(x.cur), first(x.first), last(x.last), node(x.node) { }

    reference operator*() const {
        return *cur;
    }
    pointer operator->() const {
        return &(operator*());
    }

    difference_type operator-(const self& x) const {
        return difference_type(buffer_size()) * (node - x.node - 1) +
            (cur - first) + (x.last - x.cur);
    }

    self& operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            cur += n;
        } else {
            difference_type node_offset =
                offset > 0 ? offset / difference_type(buffer_size())
                           : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) {
        return *this += -n;
    }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    bool operator==(const self& x) const {
        return cur == x.cur;
    }
    bool operator!=(const self& x) const {
        return !(*this == x);
    }
    bool operator<(const self& x) const {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }

    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
};

template <typename T, typename Ref, typename Ptr, size_t BufSize>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
    return random_access_iterator_tag();
}

template <typename T, typename Ref, typename Ptr, size_t BufSize>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
    return 0;
}

template <typename T, typename Ref, typename Ptr, size_t BufSize>
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
    return 0;
}

template <typename T, typename Alloc = alloc, size_t BufSize = 0>
class deque {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef __deque_iterator<T, T&, T*, BufSize> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSize> const_iterator;

    iterator begin() {
        return start;
    }
    const_iterator begin() const{
        return start;
    }
    iterator end() {
        return finish;
    }
    const_iterator end() const {
        return finish;
    }

    reference operator[](size_type n) {
        return start[difference_type(n)];
    }
    const_reference operator[](size_type n) const {
        return start[difference_type(n)];
    }

    reference front() {
        return *start;
    }
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    const_reference front() const {
        return *start;
    }
    const_reference back() const {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    
    size_type size() const {
        return finish - start;
    }
    size_type max_size() const {
        return size_type(-1);
    }
    bool empty() const {
        return start == finish;
    }

    deque() : start(), finish(), map(nullptr), map_size(0) {
        create_map_and_nodes(0);
    }
    deque(const deque& x) : start(), finish(),
        map(nullptr), map_size(0) {
        create_map_and_nodes(x.size());
        try {
            uninitialized_copy(x.begin(), x.end(), start);
        } catch (...) {
            destroy_map_and_nodes();
            throw;
        }
    }
    deque(size_type n, const value_type& value) : start(), finish(),
        map(nullptr), map_size(0) {
        fill_initialize(n, value);
    }
    deque(int n, const value_type& value) : start(), finish(),
        map(nullptr), map_size(0) {
        fill_initialize(n, value);
    }
    deque(long n, const value_type& value) : start(), finish(),
        map(nullptr), map_size(0) {
        fill_initialize(n, value);
    }
    explicit deque(size_type n) : start(), finish(), map(nullptr),
        map_size(0) {
        fill_initialize(n, value_type());
    }
    template <typename InputIterator>
    deque(InputIterator first, InputIterator last) : start(), finish(),
        map(nullptr), map_size(0) {
        range_initialize(first, last, iterator_category(first));

    }
    ~deque() {
        destroy(start, finish);
        destroy_map_and_nodes();
    }

    deque& operator=(const deque& x) {
        const size_type len = size();
        if (&x != this) {
            if (len > x.size()) {
                erase(std::copy(x.begin(), x.end(), start), finish);
            } else {
                const_iterator mid = x.begin() + difference_type(len);
                std::copy(x.begin(), mid, start);
                insert(finish, mid, x.end());
            }
        }
        return *this;
    }

    void swap(deque& x) {
        std::swap(start, x.start);
        std::swap(finish, x.finish);
        std::swap(map, x.map);
        std::swap(map_size, x.map_size);
    }

    void push_back(const value_type& t) {
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, t);
            ++finish.cur;
        } else {
            push_back_aux(t);
        }
    }

    void push_front(const value_type& t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        } else {
            push_front_aux(t);
        }
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur);
        } else {
            pop_back_aux();
        }
    }

    void pop_front() {
        if (start.cur != start.last - 1) {
            destroy(start.cur);
            ++start.cur;
        } else {
            pop_front_aux();
        }
    }

    iterator insert(iterator position, const value_type& x) {
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        } else if (position.cur == finish.cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else {
            return insert_aux(position, x);
        }
    }

    iterator insert(iterator position) {
        return insert(position, value_type());
    }

    void insert(iterator pos, size_type n, const value_type& x);
    void insert(iterator pos, int n, const value_type& x) {
        insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const value_type& x) {
        insert(pos, (size_type)n, x);
    }

    void resize(size_type new_size, const value_type& x) {
        const size_type len = size();
        if (new_size < len) {
            erase(start + new_size, finish);
        } else {
            insert(finish, new_size - len, x);
        }
    }

    void resize(size_type new_size) {
        resize(new_size, value_type());
    }

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if ((size_type)index < (size() >> 1)) {
            std::copy_backward(start, pos, next);
            pop_front();
        } else {
            std::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    iterator erase(iterator first, iterator last);
    void clear();

    bool operator==(const deque<T, Alloc, 0>& x) const {
        return size() == x.size() && std::equal(begin(), end(), x.begin());
    }
    bool operator!=(const deque<T, Alloc, 0>& x) const {
        return size() != x.size() || !std::equal(begin(), end(), x.begin());
    }
    bool operator<(const deque<T, Alloc, 0>& x) const {
        return std::lexicographical_compare(begin(), end(), x.begin(), x.end());
    }

protected:
    typedef pointer* map_pointer;
    typedef simple_alloc<value_type, Alloc> data_allocator;
    typedef simple_alloc<pointer, Alloc> map_allocator;

    iterator start;
    iterator finish;

    map_pointer map;
    size_type map_size;

    static size_type buffer_size() {
        return __deque_buf_size(BufSize, sizeof(value_type));
    }
    static size_type initial_map_size() {
        return 8;
    }

    void create_map_and_nodes(size_type num_elements);
    void destroy_map_and_nodes();
    void fill_initialize(size_type n, const value_type& value);

    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                          input_iterator_tag);
    template <typename ForwardIterator>
    void range_initialize(ForwardIterator first, ForwardIterator last,
                          forward_iterator_tag);

    void push_back_aux(const value_type& t);
    void push_front_aux(const value_type& t);
    void pop_back_aux();
    void pop_front_aux();

    iterator insert_aux(iterator pos, const value_type& x);
    void insert_aux(iterator pos, size_type n, const value_type& x);

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map)) {
            reallocate_map(nodes_to_add, false);
        }
    }
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > size_type(start.node - map)) {
            reallocate_map(nodes_to_add, true);
        }
    }

    iterator reserve_elements_at_front(size_type n) {
        size_type vacancies = start.cur - start.first;
        if (n > vacancies) {
            new_elements_at_front(n);
        }
        return start - difference_type(n);
    }

    iterator reserve_elements_at_back(size_type n) {
        size_type vacancies = (finish.last - finish.cur) - 1;
        if (n > vacancies) {
            new_elements_at_back(n);
        }
        return finish + difference_type(n);
    }

    void new_elements_at_front(size_type new_elements);
    void new_elements_at_back(size_type new_elements);

    void destroy_nodes_at_front(iterator before_start);
    void destroy_nodes_at_back(iterator after_finish);

    void reallocate_map(size_type nodes_to_add, bool add_at_front);

    pointer allocate_node() {
        return data_allocator::allocate(buffer_size());
    }
    void deallocate_node(pointer n) {
        data_allocator::deallocate(n, buffer_size());
    }
};

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos, size_type n,
                                      const value_type& x) {
    if (pos.cur == start.cur) {
        iterator new_start = reserve_elements_at_front(n);
        uninitialized_fill(new_start, start, x);
        start = new_start;
    } else if (pos.cur == finish.cur) {
        iterator new_finish = reserve_elements_at_back(n);
        uninitialized_fill(finish, new_finish, x);
        finish = new_finish;
    } else {
        insert_aux(pos, n, x);
    }
}

template <typename T, typename Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
    assert(first < last);

    if (first == start && last == finish) {
        clear();
        return finish;
    } else {
        difference_type n = last - first;
        difference_type elems_before = first - start;
        if ((size_type)elems_before < (size() - n) / 2) {
            std::copy_backward(start, first, last);
            iterator new_start = start + n;
            destroy(start, new_start);
            for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
                data_allocator::deallocate(*cur, buffer_size());
            }
            start = new_start;
        } else {
            std::copy(last, finish, first);
            iterator new_finish = finish - n;
            destroy(new_finish, finish);
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
                data_allocator::deallocate(*cur, buffer_size());
            }
            finish = new_finish;
        }
        return start + elems_before;
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
    for (map_pointer node = start.node + 1; node < finish.node; ++node) {
        destroy(*node, *node + buffer_size());
        data_allocator::deallocate(*node, buffer_size());
    }

    if (start.node != finish.node) {
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        data_allocator::deallocate(finish.first, buffer_size());
    } else {
        destroy(start.cur, finish.cur);
    }

    finish = start;
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
    size_type num_nodes = num_elements / buffer_size() + 1;

    map_size = std::max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);

    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur) {
            *cur = allocate_node();
        }
    } catch (...) {
        for (map_pointer n = nstart; n < cur; ++n) {
            deallocate_node(*n);
        }
        map_allocator::deallocate(map, map_size);
        throw;
    }

    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % buffer_size();
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {
    for (map_pointer cur = start.node; cur <= finish.node; ++cur) {
        deallocate_node(*cur);
    }
    map_allocator::deallocate(map, map_size);
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
    create_map_and_nodes(n);
    map_pointer cur;
    try {
        for (cur = start.node; cur < finish.node; ++cur) {
            uninitialized_fill(*cur, *cur + buffer_size(), value);
        }
        uninitialized_fill(finish.first, finish.cur, value);
    } catch (...) {
        for (map_pointer n = start.node; n < cur; ++n) {
            destroy(*n, *n + buffer_size());
        }
        destroy_map_and_nodes();
        throw;
    }
}

template <typename T, typename Alloc, size_t BufSize>
template <typename InputIterator>
void deque<T, Alloc, BufSize>::range_initialize(InputIterator first,
                                                InputIterator last,
                                                input_iterator_tag) {
    create_map_and_nodes(0);
    for (; first != last; ++first) {
        push_back(*first);
    }
}

template <typename T, typename Alloc, size_t BufSize>
template <typename ForwardIterator>
void deque<T, Alloc, BufSize>::range_initialize(ForwardIterator first,
                                                ForwardIterator last,
                                                forward_iterator_tag) {
    size_type n = 0;
    distance(first, last, n);
    create_map_and_nodes(n);
    try {
        uninitialized_copy(first, last, start);
    } catch (...) {
        destroy_map_and_nodes();
        throw;
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
    assert(finish.cur == finish.last - 1);

    value_type t_copy = t;
    reserve_map_at_back();
    *(finish.node + 1) = allocate_node();
    try {
        construct(finish.cur, t_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.last;
    } catch (...) {
        deallocate_node(*(finish.node + 1));
        throw;
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
    assert(start.cur == start.first);

    value_type t_copy = t;
    reserve_map_at_front();
    *(start.node - 1) = allocate_node();
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        construct(start.cur, t_copy);
    } catch (...) {
        start.set_node(start.node + 1);
        start.cur = start.first;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux() {
    assert(finish.cur == finish.first);
    deallocate_node(finish.first);
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
    assert(start.cur == start.last - 1);
    destroy(start.cur);
    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template <typename T, typename Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
    difference_type index = pos - start;
    value_type x_copy = x;
    if ((size_type)index < size() / 2) {
        push_front(front());
        iterator front1 = start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        ++pos1;
        std::copy(front2, pos1, front1);
    } else {
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + index;
        std::copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos, size_type n,
                                          const value_type& x) {
    const difference_type elems_before = pos - start;
    size_type length = size();
    value_type x_copy = x;
    if ((size_type)elems_before < length / 2) {
        iterator new_start = reserve_elements_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        try {
            if (elems_before >= difference_type(n)) {
                iterator start_n = start + difference_type(n);
                uninitialized_copy(start, start_n, new_start);
                start = new_start;
                std::copy(start_n, pos, old_start);
                std::fill(pos - difference_type(n), pos, x_copy);
            } else {
                __uninitialized_copy_fill(start, pos, new_start, start, x_copy);
                start = new_start;
                std::fill(old_start, pos, x_copy);
            }
        } catch (...) {
            destroy_nodes_at_front(new_start);
            throw;
        }
    } else {
        iterator new_finish = reserve_elements_at_back(n);
        iterator old_finish = finish;
        const difference_type elems_after = difference_type(length) - elems_before;
        pos = finish - elems_after;
        try {
            if (elems_after > difference_type(n)) {
                iterator finish_n = finish - difference_type(n);
                uninitialized_copy(finish_n, finish, finish);
                finish = new_finish;
                std::copy_backward(pos, finish_n, old_finish);
                std::fill(pos, pos + difference_type(n), x_copy);
            }
            else {
                __uninitialized_fill_copy(finish, pos + difference_type(n),
                                          x_copy, pos, finish);
                finish = new_finish;
                std::fill(pos, old_finish, x_copy);
            }
        } catch (...) {
            destroy_nodes_at_back(new_finish);
        }
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) {
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_front(new_nodes);
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i) {
            *(start.node - i) = allocate_node();
        }
    } catch (...) {
        for (size_type j = 1; j < i; ++i) {
            deallocate_node(*(start.node - j));
            throw;
        }
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_back(new_nodes);
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i) {
            *(finish.node + i) = allocate_node();
        }
    } catch (...) {
        for (size_type j = 1; j < i; ++i) {
            deallocate_node(*(finish.node + 1));
        }
        throw;
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {
    for (map_pointer n = before_start.node; n < start.node; ++n) {
        deallocate_node(*n);
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) {
    for (map_pointer n = after_finish.node; n > finish.node; --n) {
        deallocate_node(*n);
    }
}

template <typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add,
                                              bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_nstart;
    if (map_size > 2 * new_num_nodes) {
        new_nstart = map + (map_size - new_num_nodes) / 2
            + (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node) {
            std::copy(start.node, finish.node + 1, new_nstart);
        } else {
            std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
    } else {
        size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;

        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2
            + (add_at_front ? nodes_to_add : 0);
        std::copy(start.node, finish.node + 1, new_nstart);
        map_allocator::deallocate(map, map_size);

        map = new_map;
        map_size = new_map_size;
    }

    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_DEQUE_H_

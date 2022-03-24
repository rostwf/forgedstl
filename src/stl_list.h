#ifndef FORGED_STL_INTERNAL_LIST_H_
#define FORGED_STL_INTERNAL_LIST_H_

#include <algorithm>
#include <cstddef>

#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_iterator.h"

namespace forgedstl {

template <typename T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*>  iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef __list_iterator<T, Ref, Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;

    __list_iterator(link_type x) : node(x) { }
    __list_iterator() { }
    __list_iterator(const iterator& x) : node(x.node) { }

    bool operator==(const self& x) const {
        return node == x.node;
    }
    bool operator!=(const self& x) const {
        return node != x.node;
    }
    reference operator*() const {
        return (*node).data;
    }
    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template <typename T, typename Ref, typename Ptr>
inline bidirectional_iterator_tag
iterator_category(const __list_iterator<T, Ref, Ptr>&) {
    return bidirectional_iterator_tag();
}

template <typename T, typename Ref, typename Ptr>
inline T* value_type(const __list_iterator<T, Ref, Ptr>&) {
    return 0;
}

template <typename T, typename Ref, typename Ptr>
inline ptrdiff_t* distance_type(const __list_iterator<T, Ref, Ptr>&) {
    return 0;
}

template <typename T, typename Alloc = alloc>
class list {
protected:
    typedef void* void_pointer;
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;

public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator> reverse_iterator;

    list() {
        empty_initialize();
    }
    list(size_type n, const T& value) {
        fill_initialize(n, value);
    }
    list(int n, const T& value) {
        fill_initialize(n, value);
    }
    list(long n, const T& value) {
        fill_initialize(n, value);
    }
    explicit list(size_type n) {
        fill_initialize(n, T());
    }
    template <typename InputIterator>
    list(InputIterator first, InputIterator last) {
        range_initialize(first, last);
    }
    list(const list<T, Alloc>& x) {
        range_initialize(x.begin(), x.end());
    }
    ~list() {
        clear();
        put_node(node);
    }
    list<T, Alloc>& operator=(const list<T, Alloc>& x);

    iterator begin() {
        return (link_type)(node->next);
    }
    const_iterator begin() const {
        return (link_type)(node->next);
    }
    iterator end() {
        return node;
    }
    const_iterator end() const{
        return node;
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    bool empty() const {
        return node->next == node;
    }
    size_type size() const {
        size_type n = 0;
        distance(begin(), end(), n);
        return n;
    }
    size_type max_size() const {
        return size_type(-1);
    }
    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }
    reference back() {
        return *(--end());
    }
    const_reference back() const {
        return *(--end());
    }
    void swap(list<T, Alloc>& x) {
        std::swap(node, x.node);
    }
    iterator insert(iterator position, const T& x) {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }
    iterator insert(iterator position) {
        return insert(position, T());
    }
    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
    void insert(iterator position, size_type n, const T& x);
    void insert(iterator position, int n, const T& x) {
        insert(position, (size_type)n, x);
    }
    void insert(iterator position, long n, const T& x) {
        insert(position, (size_type)n, x);
    }

    void push_front(const T& x) {
        insert(begin(), x);
    }
    void push_back(const T& x) {
        insert(end(), x);
    }
    iterator erase(iterator position) {
        if (position.node != node) {
            link_type next_node = link_type(position.node->next);
            link_type prev_node = link_type(position.node->prev);
            next_node->prev = prev_node;
            prev_node->next = next_node;
            destroy_node(position.node);
            return iterator(next_node);
        } else {
            return end();
        }
    }
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) {
        resize(new_size, T());
    }
    void clear();

    void pop_front() {
        erase(begin());
    }
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

    void splice(iterator position, list& x) {
        if (!x.empty()) {
            transfer(position, x.begin(), x.end());
        }
    }
    void splice(iterator position, list&, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) {
            return;
        }
        transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first, iterator last) {
        if (first != last) {
            transfer(position, first, last);
        }
    }
    void remove(const T& value);
    void unique();
    void merge(list& x);
    void reverse();
    void sort();

    template <typename Predicate>
    void remove_if(Predicate);
    template <typename BinaryPredicate>
    void unique(BinaryPredicate);
    template <typename StrictWeakOrdering>
    void merge(list&, StrictWeakOrdering);
    template <typename StrictWeakOrdering>
    void sort(StrictWeakOrdering);

    friend bool operator==<>(const list& x, const list& y);

protected:
    link_type node;

    link_type get_node() {
        return list_node_allocator::allocate();
    }
    void put_node(link_type p) {
        list_node_allocator::deallocate(p);
    }

    link_type create_node(const T& x) {
        link_type p = get_node();
        try {
            construct(&p->data, x);
        } catch (...) {
            put_node(p);
        }
        return p;
    }
    void destroy_node(link_type p) {
        destroy(p);
        put_node(p);
    }

    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }
    void fill_initialize(size_type n, const T& value) {
        empty_initialize();
        try {
            insert(begin(), n, value);
        } catch (...) {
            clear();
            put_node(node);
            throw;
        }
    }
    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        empty_initialize();
        try {
            insert(begin(), first, last);
        } catch (...) {
            clear();
            put_node(node);
            throw;
        }
    }

    void transfer(iterator position, iterator first, iterator last) {
        (link_type(last.node->prev))->next = position.node;
        (link_type(first.node->prev))->next = last.node;
        (link_type(position.node->prev))->next = first.node;
        link_type tmp = link_type(position.node->prev);
        position.node->prev = last.node->prev;
        last.node->prev = first.node->prev;
        first.node->prev = tmp;
    }
};

template <typename T, typename Alloc>
inline bool operator==(const list<T, Alloc>& x, const list<T, Alloc>& y) {
    typedef typename list<T, Alloc>::link_type link_type;
    link_type end1 = x.node;
    link_type end2 = y.node;
    link_type node1 = (link_type)end1->next;
    link_type node2 = (link_type)end2->next;
    for (; node1 != end1 && node2 != end2;
         node1 = (link_type)node1->next, node2 = (link_type)node2->next) {
        if (node1->data != node2->data) {
            return false;
        }
    }
    return node1 == end1 && node2 == end2;
}

template <typename T, typename Alloc>
inline bool operator<(const list<T, Alloc>& x, const list<T, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Alloc>
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
    x.swap(y);
}

template <typename T, typename Alloc>
template <typename InputIterator>
void list<T, Alloc>::insert(iterator position,
                            InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
        insert(position, *first);
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    for (; n > 0; --n) {
        insert(position, x);
    }
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last) {
        erase(first);
        ++first;
    }
    return last;
}

template <typename T, typename Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x) {
    iterator iter = begin();
    size_type len = 0;
    for (; iter != end() && len < new_size; ++iter, ++len) { }
    if (len == new_size) {
        erase(iter, end());
    } else {
        insert(end(), new_size - len, x);
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::clear() {
    link_type cur = (link_type)node->next;
    while (cur != node) {
        link_type del = cur;
        cur = (link_type)cur->next;
        destroy_node(del);
    }
    node->next = node;
    node->prev = node;
}

template <typename T, typename Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& x) {
    if (this != &x) {
        iterator node1 = begin();
        iterator last1 = end();
        const_iterator node2 = x.begin();
        const_iterator last2 = x.end();
        while (node1 != last1 && node2 != last2) {
            *node1++ = *node2++;
        }
        if (node2 == last2) {
            erase(node1, last1);
        } else {
            insert(last1, node2, last2);
        }
    }
    return *this;
}

template <typename T, typename Alloc>
void list<T, Alloc>::remove(const T& value) {
    iterator iter = begin();
    iterator last = end();
    while (iter != last) {
        iterator next = iter;
        ++next;
        if (*iter == value) {
            erase(iter);
        }
        iter = next;
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::unique() {
    iterator iter = begin();
    iterator last = end();
    if (iter == last) {
        return;
    }

    iterator next = iter;
    while (++next != last) {
        if (*iter == *next) {
            erase(next);
            next = iter;
        } else {
            iter = next;
        }
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
    iterator iter1 = begin();
    iterator last1 = end();
    iterator iter2 = x.begin();
    iterator last2 = x.end();
    while (iter1 != last1 && iter2 != last2) {
        if (*iter2 < *iter1) {
            iterator next = iter2;
            transfer(iter1, iter2, ++next);
            iter2 = next;
        } else {
            ++iter1;
        }
    }
    if (iter2 != last2) {
        transfer(last1, iter2, last2);
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::reverse() {
    if (node->next == node || link_type(node->next)->next == node) {
        return;
    }
    iterator iter = begin();
    ++iter;
    while (iter != end()) {
        iterator old = iter;
        ++iter;
        transfer(begin(), old, iter);
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::sort() {
    if (node->next == node || link_type(node->next)->next == node) {
        return;
    }
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i]);
            ++i;
        }
        carry.swap(counter[i]);
        if (i == fill) {
            ++fill;
        }
    }

    for (int i = 1; i < fill; ++i) {
        counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
}

template <typename T, typename Alloc>
template <typename Predicate>
void list<T, Alloc>::remove_if(Predicate pred) {
    iterator iter = begin();
    iterator last = end();
    while (iter != last) {
        iterator next = iter;
        ++next;
        if (pred(*iter)) {
            erase(iter);
        }
        iter = next;
    }
}

template <typename T, typename Alloc>
template <typename BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
    iterator iter = begin();
    iterator last = end();
    if (iter == last) {
        return;
    }
    iterator next = iter;
    while (++next != last) {
        if (binary_pred(*iter, *next)) {
            erase(next);
            next = iter;
        } else {
            iter = next;
        }
    }
}

template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void list<T, Alloc>::merge(list<T, Alloc>& x, StrictWeakOrdering comp) {
    iterator iter1 = begin();
    iterator last1 = end();
    iterator iter2 = x.begin();
    iterator last2 = x.end();
    while (iter1 != last1 && iter2 != last2) {
        if (comp(*iter2, *iter1)) {
            iterator next = iter2;
            ++next;
            transfer(iter1, iter2, next);
            iter2 = next;
        } else {
            ++iter1;
        }
    }
    if (iter2 != last2) {
        transfer(last1, iter2, last2);
    }
}

template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void list<T, Alloc>::sort(StrictWeakOrdering comp) {
    if (node->next == node || link_type(node->next)->next == node) {
        return;
    }
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry, comp);
            carry.swap(counter[i]);
            ++i;
        }
        carry.swap(counter[i]);
        if (i == fill) {
            ++fill;
        }
    }
    for (int i = 1; i < fill; ++i) {
        counter[i].merge(counter[i - 1], comp);
    }
    swap(counter[fill - 1]);
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_LIST_H_

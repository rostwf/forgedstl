#ifndef FORGED_STL_INTERNAL_VECTOR_H_
#define FORGED_STL_INTERNAL_VECTOR_H_

#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_iterator.h"
#include "stl_uninitialized.h"

namespace forgedstl {

template <typename T, typename Alloc = alloc>
class vector {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator> reverse_iterator;

    iterator begin() {
        return start;
    }
    const_iterator begin() const {
        return start;
    }
    iterator end() {
        return finish;
    }
    const_iterator end() const {
        return finish;
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

    size_type size() const {
        return size_type(end() - begin());
    }
    size_type max_size() const {
        return size_type(-1) / sizeof(T);
    }
    size_type capacity() const {
        return size_type(end_of_storage - begin());
    }
    bool empty() const {
        return begin() == end();
    }
    reference operator[](size_type n) {
        return *(begin() + n);
    }
    const_reference operator[](size_type n) const {
        return *(begin() + n);
    }

    // constructor
    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) { }
    vector(size_type n, const T& value) {
        fill_initialize(n, value);
    }
    vector(int n, const T& value) {
        fill_initialize(n, value);
    }
    vector(long n, const T& value) {
        fill_initialize(n, value);
    }
    explicit vector(size_type n) {
        fill_initialize(n, T());
    }

    vector(const vector<T, Alloc>& x) {
        start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
        finish = start + (x.end() - x.begin());
        end_of_storage = finish;
    }

    template <typename InputIterator>
    vector(InputIterator first, InputIterator last) :
        start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        range_initialize(first, last, iterator_category(first));
    }
    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    vector<T, Alloc>& operator=(const vector<T, Alloc>& x);

    void reserve(size_type n) {
        if (capacity() < n) {
            const size_type old_size = size();
            iterator tmp = allocate_and_copy(n, start, finish);
            destroy(start, finish);
            deallocate();
            start = tmp;
            finish = start + old_size;
            end_of_storage = start + n;
        }
    }

    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }
    reference back() {
        return *(end() - 1);
    }
    const_reference back() const {
        return *(end() - 1);
    }

    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else {
            insert_aux(end(), x);
        }
    }

    void swap(vector<T, Alloc>& x) { // todo
        std::swap(start, x.start);
        std::swap(finish, x.finish);
        std::swap(end_of_storage, x.end_of_storage);
    }

    iterator insert(iterator position, const T& x) {
        size_type n = position - begin();
        if (finish != end_of_storage && position == end()) {
            construct(finish, x);
            ++finish;
        } else {
            insert_aux(position, x);
        }
        return begin() + n;
    }
    iterator insert(iterator position) {
        return insert(position, T());
    }
    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        range_insert(position, first, last, iterator_category(first));
    }
    void insert(iterator position, size_type n, const T& x);
    void insert(iterator position, int n, const T& x) {
        insert(position, (size_type)n, x);
    }
    void insert(iterator position, long n, const T& x) {
        insert(position, (size_type)n, x);
    }

    void pop_back() {
        --finish;
        destroy(finish);
    }

    iterator erase(iterator position) {
        if (position + 1 != end()) {
            std::copy(position + 1, finish, position);
        }
        --finish;
        destroy(finish);
        return position;
    }
    iterator erase(iterator first, iterator last) {
        iterator i = std::copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }

    void resize(size_type new_size, const T& x) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), x);
        }
    }
    void resize(size_type new_size) {
        resize(new_size, T());
    }

    void clear() {
        erase(begin(), end());
    }

protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator position, const T& x);

    void deallocate() {
        if (start != nullptr) {
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }

    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = data_allocator::allocate(n);
        try {
            uninitialized_fill_n(result, n, value);
            return result;
        } catch(...) {
            data_allocator::deallocate(result, n);
            throw;
        }
    }

    template <typename InputIterator>
    iterator allocate_and_copy(size_type n,
                               InputIterator first, InputIterator last) {
        iterator result = data_allocator::allocate(n);
        try {
            uninitialized_copy(first, last, result);
            return result;
        } catch (...) {
            data_allocator::deallocate(result, n);
            throw;
        }
    }

#if 0
    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                          input_iterator_tag) {
        for (; first != last; ++first) {
            push_back(*first);
        }
    }
#endif // 0

    template <typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                          input_iterator_tag) {
        size_type n = distance(first, last);
        start = allocate_and_copy(n, first, last);
        finish = start + n;
        end_of_storage = finish;
    }

    template <typename InputIterator>
    void range_insert(iterator pos,
                      InputIterator first, InputIterator last,
                      input_iterator_tag);
    template <typename ForwardIterator>
    void range_insert(iterator pos,
                      ForwardIterator first, ForwardIterator last,
                      forward_iterator_tag);

};

template <typename T, typename Alloc>
inline bool operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename Alloc>
inline bool operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Alloc>
inline void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) {
    x.swap(y);
}

template <typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
    if (this != &x) {
        if (x.size() > capacity()) {
            iterator tmp = allocate_and_copy(x.end() - x.begin(),
                                             x.begin(), x.end());
            destroy(start, finish);
            deallocate();
            start = tmp;
            end_of_storage = start + (x.end() - x.begin());
        } else if (size() > x.size()) {
            iterator i = std::copy(x.begin(), x.end(), begin());
            destroy(i, end());
        } else {
            std::copy(x.begin(), x.begin() + size(), begin());
            uninitialized_copy(x.begin() + size(), x.end(), end());
        }
        finish = start + x.size();
    }
    return *this;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
    if (finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        T x_copy = x;
        std::copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    } else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        } catch (...) {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        destroy(begin(), end());
        deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + len;
    }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    if (n != 0) {
        if (size_type(end_of_storage - finish) >= n) {
            T x_copy = x;
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                std::copy_backward(position, old_finish - n, old_finish);
                std::fill(position, position + n, x_copy);
            } else {
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                std::fill(position, old_finish, x_copy);
            }
        } else {
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                new_finish = uninitialized_fill_n(new_finish, n, x);
                new_finish = uninitialized_copy(position, finish, new_finish);
            } catch (...) {
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;
        }
    }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void vector<T, Alloc>::range_insert(iterator position,
                                    InputIterator first, InputIterator last,
                                    input_iterator_tag) {
    for (; first != last; ++first) {
        position = insert(position, *first);
        ++position;
    }
}

template <typename T, typename Alloc>
template <typename ForwardIterator>
void vector<T, Alloc>::range_insert(iterator position,
                                    ForwardIterator first, ForwardIterator last,
                                    forward_iterator_tag) {
    if (first != last) {
        size_type n = distance(first, last);
        if (size_type(end_of_storage - finish) >= n) {
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                std::copy_backward(position, finish - n, old_finish);
                std::copy(first, last, position);
            } else {
                ForwardIterator mid = first;
                advance(mid, elems_after);
                uninitialized_copy(mid, last, finish);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                std::copy(first, mid, position);
            }
        } else {
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                new_finish = uninitialized_copy(first, last, new_finish);
                new_finish = uninitialized_copy(position, finish, new_finish);
            } catch (...) {
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;
        }
    }
}

} // namepsace forgedstl

#endif // FORGED_STL_INTERNAL_VECTOR_H_

#ifndef FORGED_STL_INTERNAL_UNINITIALIZED_H_
#define FORGED_STL_INTERNAL_UNINITIALIZED_H_

#include "stl_construct.h"
#include "stl_iterator.h"
#include "type_traits.h"

#include <algorithm>
#include <iterator>

namespace forgedstl {

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator 
__uninitialized_fill_n_aux(ForwardIterator first, Size n,
                           const T& x, __true_type) {
    return std::fill_n(first, n, x); // todo
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n,
                           const T& x, __false_type) {
    ForwardIterator cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            construct(&*cur, x);
        }
        return cur;
    } catch (...) {
        destroy(first, cur);
        throw;
    }
}

template <typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n,
                                              const T& x, T1*) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
                                            Size n, const T& x) {
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __true_type) {
    return std::copy(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
        return cur;
    } catch (...) {
        destroy(result, cur);
        throw;
    }
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last,
                     ForwardIterator result, T*) {
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}

inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                   wchar_t* result) {
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <typename ForwardIterator, typename T>
inline void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                         const T& x, __true_type) {
    std::fill(first, last, x);
}

template <typename ForwardIterator, typename T>
inline void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                         const T& x, __false_type) {
    ForwardIterator cur = first;
    try {
        for (; cur != last; ++cur) {
            construct(&*cur, x);
        }
    } catch (...) {
        destroy(first, cur);
        throw;
    }
}

template <typename ForwardIterator, typename T, typename T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                 const T& x, T1*) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, 
                               ForwardIterator last, const T& x) {
    __uninitialized_fill(first, last, x, value_type(first));
}

template <typename ForwardIterator, typename T, typename InputIterator>
inline ForwardIterator
__uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& x,
                          InputIterator first, InputIterator last) {
    uninitialized_fill(result, mid, x);
    try {
        return uninitialized_copy(first, last, mid);
    } catch (...) {
        destroy(result, mid);
        throw;
    }
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline void __uninitialized_copy_fill(InputIterator first1, InputIterator last1,
                                      ForwardIterator first2, ForwardIterator last2,
                                      const T& x) {
    ForwardIterator mid2 = uninitialized_copy(first1, last1, first2);
    try {
        uninitialized_fill(mid2, last2, x);
    } catch (...) {
        destroy(first2, mid2);
        throw;
    }
}

} // namepsace forgedstl

#endif // FORGED_STL_INTERNAL_UNINITIALIZED_H_

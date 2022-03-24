#ifndef FORGED_STL_INTERNAL_CONSTRUCT_H_
#define FORGED_STL_INTERNAL_CONSTRUCT_H_

#include <new>

#include "type_traits.h"
#include "stl_iterator.h"

namespace forgedstl {

template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);
}

template <typename T>
inline void destroy(T* pointer) {
    pointer->~T();
}

template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for (; first < last; ++first) {
        destroy(&*first);
    }
}

template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T* t) {
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}

inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

}

#endif // FORGED_STL_INTERNAL_CONSTRUCT_H_

#ifndef FORGED_STL_INTERNAL_HEAP_H_
#define FORGED_STL_INTERNAL_HEAP_H_

#include "stl_iterator.h"

namespace forgedstl {

template <typename RandomAccessIterator, typename Distance, typename T>
void __push_heap(RandomAccessIterator first, Distance holeIndex,
                 Distance topIndex, T value) {
    Distance parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && *(first + parent) < value) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

#if 0

template <typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    __push_heap(first, difference_type((last - first) - 1), difference_type(0), value_type(*(last - 1)));
}

#else // #if 0

template <typename RandomAccessIterator, typename Distance, typename T>
inline void __push_heap_aux(RandomAccessIterator first,
                            RandomAccessIterator last,
                            Distance*, T*) {
    __push_heap(first, Distance((last - first) - 1),
                Distance(0), T(*(last - 1)));
}

template <typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

#endif // #if 0


template <typename RandomAccessIterator, typename Distance,
          typename T, typename Compare>
inline void __push_heap(RandomAccessIterator first, Distance holeIndex,
                            Distance topIndex, T value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), value)) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <typename RandomAccessIterator, typename Compare, 
          typename Distance, typename T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                                Compare comp, Distance*, T*) {
    __push_heap(first, Distance((last - first) - 1), Distance(0),
                T(*(last - 1)), comp);
}

template <typename RandomAccessIterator, typename Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last,
                      Compare comp) {
    __push_heap_aux(first, last, comp, distance_type(first), value_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
                   Distance len, T value) {
    Distance topIndex = holeIndex;
    Distance child = 2 * (holeIndex + 1);
    while (child < len) {
        if (*(first + child) < *(first + (child - 1))) {
            --child;
        }
        *(first + holeIndex) = *(first + child);
        holeIndex = child;
        child = 2 * (child + 1);
    }
    if (child == len) {
        *(first + holeIndex) = *(first + (child - 1));
        holeIndex = child - 1;
    }
    __push_heap(first, holeIndex, topIndex, value);
}

template <typename RandomAccessIterator, typename T, typename Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator result, T value, Distance*) {
    *result = *first;
    __adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <typename RandomAccessIterator, typename T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}

template <typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    __pop_heap_aux(first, last, value_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
                   Distance len, T value, Compare comp) {
    Distance topIndex = holeIndex;
    Distance child = 2 * (holeIndex + 1);
    while (child < len) {
        if (comp(*(first + child), *(first + (child - 1)))) {
            --child;
        }
        *(first + holeIndex) = *(first + child);
        holeIndex = child;
        child = 2 * (child + 1);
    }
    if (child == len) {
        *(first + holeIndex) = *(first + (child - 1));
        holeIndex = child - 1;
    }
    __push_heap(first, holeIndex, topIndex, value, comp);
}

template <typename RandomAccessIterator, typename T, typename Compare, typename Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator result, T value, Compare comp,
                       Distance*) {
    *result = *first;
    __adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <typename RandomAccessIterator, typename T, typename Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                           T*, Compare comp) {
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp,
               distance_type(first));
}

template <typename RandomAccessIterator, typename Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                     Compare comp) {
    __pop_heap_aux(first, last, value_type(first), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T>
inline void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
    if (last - first < 2) {
        return;
    }
    Distance len = last - first;
    Distance holeIndex = (len - 1 - 1) / 2;

    while (true) {
        __adjust_heap(first, holeIndex, len, *(first + holeIndex));
        if (holeIndex == 0) {
            return;
        }
        --holeIndex;
    }
}

template <typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    __make_heap(first, last, distance_type(first), value_type(first));
}

template <typename RandomAccessIterator, typename Compare, typename Distance, typename T>
inline void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp,
                        Distance*, T*) {
    if (last - first < 2) {
        return;
    }
    Distance len = last - first;
    Distance holeIndex = (len - 1 - 1) / 2;

    while (true) {
        __adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        if (holeIndex == 0) {
            return;
        }
        --holeIndex;
    }
}

template <typename RandomAccessIterator, typename Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    __make_heap(first, last, comp, distance_type(first), value_type(first));
}

template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    while (last - first > 1) {
        pop_heap(first, last--);
    }
}

template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    while (last - first > 1) {
        pop_heap(first, last--, comp);
    }
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_HEAP_H_

#ifndef FORGED_STL_INTERNAL_ITERATOR_H_
#define FORGED_STL_INTERNAL_ITERATOR_H_

#include <cstddef>

namespace forgedstl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename T, typename Distence>
struct input_iterator {
    typedef input_iterator_tag iterator_category;
    typedef T                  value_type;
    typedef Distence           difference_type;
    typedef T*                 pointer;
    typedef T&                 reference;
};

template <typename T, typename Distence>
struct output_iterator {
    typedef output_iterator_tag iterator_category;
    typedef void                value_type;
    typedef void                difference_type;
    typedef void                pointer;
    typedef void                reference;
};

template <typename T, typename Distence>
struct bidirectional_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distence                   difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

template <typename T, typename Distence>
struct random_access_iterator {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distence                   difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

template <typename Category, typename T, typename Distence = ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
struct iterator {
    typedef Category  iterator_category;
    typedef T         value_type;
    typedef Distence  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
};

template <typename Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
};

template <typename T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

template <typename T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef const T                    value_type;
    typedef ptrdiff_t                  difference_type;
    typedef const T*                   pointer;
    typedef const T&                   reference;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distence(RandomAccessIterator first, RandomAccessIterator last,
           random_access_iterator_tag) {
    return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

template <typename InputIterator, typename Distance>
inline void __distance(InputIterator first, InputIterator last, Distance& n,
                     input_iterator_tag) {
    while (first != last) {
        ++first;
        ++n;
    }
}

template <typename InputIterator, typename Distance>
inline void __distance(InputIterator first, InputIterator last, Distance& n,
                       random_access_iterator_tag) {
    n += last - first;
}

template <typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, n, category());
}

template <typename InputIterator, typename Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) {
        ++i;
    }
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& i, Distance n,
                      bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) {
            ++i;
        }
    }
    else {
        while (n++) {
            --i;
        }
    }
}

template <typename RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n,
                      random_access_iterator_tag) {
    i += n;
}

template <typename InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
    __advance(i, n, iterator_category(i));
}

template <typename Iterator>
class reverse_iterator {
public:
    typedef typename iterator_traits<Iterator>::iterator_category
        iterator_category;
    typedef typename iterator_traits<Iterator>::value_type
        value_type;
    typedef typename iterator_traits<Iterator>::difference_type
        difference_type;
    typedef typename iterator_traits<Iterator>::pointer
        pointer;
    typedef typename iterator_traits<Iterator>::reference
        reference;
    typedef Iterator iterator_type;
    typedef reverse_iterator<Iterator> self;

    reverse_iterator() { }
    explicit reverse_iterator(iterator_type x) : current(x) { }
    reverse_iterator(const self& x) : current(x.current) { }
    template <typename Iter>
    reverse_iterator(const reverse_iterator<Iter>& x) : current(x.current) { }

    iterator_type base() {
        return current;
    }
    reference operator*() {
        Iterator tmp = current;
        return *--tmp;
    }
    pointer operator->() {
        return &(operator*());
    }

    self& operator++() {
        --current;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        --current;
        return tmp;
    }
    self& operator--() {
        ++current;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self operator+(difference_type n) const {
        return self(current - n);
    }
    self& operator+=(difference_type n) {
        current -= n;
        return *this;
    }
    self operator-(difference_type n) const {
        return self(current + n);
    }
    self& operator-=(difference_type n) {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

protected:
    Iterator current;
};

template <typename Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x,
                       const reverse_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
inline bool operator<(const reverse_iterator<Iterator>& x,
                      const reverse_iterator<Iterator>& y) {
    return x.base() < y.base();
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& x,
          const reverse_iterator<Iterator>& y) {
    return y.base() - x.base();
}

template <typename Iterator>
reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n,
          const reverse_iterator<Iterator>& x) {
    return reverse_iterator<Iterator>(x.base() - n);
}

}

#endif // FORGED_STL_INTERNAL_ITERATOR_H_

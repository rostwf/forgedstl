#ifndef FORGED_STL_INTERNAL_MAP_H_
#define FORGED_STL_INTERNAL_MAP_H_

#include "stl_function.h"
#include "stl_pair.h"
#include "stl_tree.h"

namespace forgedstl {

template <typename Key, typename T, typename Compare = std::less<int>, typename Alloc = alloc>
class map {
public:
    typedef Key key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef pair<const Key, T> value_type;
    typedef Compare key_compare;

    class value_compare : public std::binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare, Alloc>;
    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) { }
    };

private:
    typedef rb_tree<key_type, value_type,
                    select1st<value_type>, key_compare, Alloc> rep_type;

    typedef typename rep_type::pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::iterator iterator;
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::reverse_iterator reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    map() : t(Compare()) { }
    explicit map(const Compare& comp) : t(comp) { }

    template <typename InputIterator>
    map(InputIterator first, InputIterator last) : t(Compare()) {
        t.insert_unique(first, last);
    }
    template <typename InputIterator>
    map(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) {
        t.insert_unique(first, last);
    }

    map(const map<Key, T, Compare, Alloc>& x) : t(x.t) { }
    map<Key, T, Compare, Alloc>& operator=(const map<Key, T, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    key_compare key_comp() const {
        return t.key_comp() :
    }
    value_compare value_comp() const {
        return value_compare(t.key_comp());
    }
    iterator begin() {
        return t.begin();
    }
    const_iterator begin() const {
        return t.begin();
    }
    iterator end() {
        return t.end();
    }
    const_iterator end() const {
        return t.end();
    }
    bool empty() const {
        return t.empty();
    }
    size_type size() const {
        return t.size();
    }
    size_type max_size() const {
        return t.max_size();
    }
    T& operator[](const key_type& k) {
        return (*((insert(value_type(k, T()))).first)).second;
    }
    void swap(map<Key, T, Compare, Alloc>& x) {
        t.swap(x.t);
    }

    pair<iterator, bool> insert(const value_type& x) {
        return t.insert_unique(x);
    }
    iterator insert(iterator position, const value_type& x) {
        return t.insert_unique(position, x);
    }
    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        t.insert_unique(first, last);
    }

    void erase(iterator position) {
        t.erase(position);
    }
    size_type erase(const key_type& x) {
        return t.erase(x);
    }
    void erase(iterator first, iterator last) {
        t.erase(first, last);
    }
    void clear() {
        t.clear();
    }

    iterator find(const key_type& x) {
        return t.find(x);
    }
    const_iterator find(const key_type& x) const {
        return t.find(x);
    }
    size_type count(const key_type& x) const {
        return t.count(x);
    }
    iterator lower_bound(const key_type& x) {
        return t.lower_bound(x);
    }
    const_iterator lower_bound(const key_type& x) const {
        return t.lower_bound(x);
    }
    iterator upper_bound(const key_type& x) {
        return t.upper_bound(x);
    }
    const_iterator upper_bound(const key_type& x) const {
        return t.upper_bound(x);
    }

    pair<iterator, iterator> equal_range(const key_type& x) {
        return t.equal_range(x);
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
        return t.equal_range(x);
    }

    friend bool operator== <> (const map&, const map&);
    friend bool operator< <> (const map&, const map&);

private:
    rep_type t;
};

template <typename Key, typename T, typename Compare, typename Alloc>
inline bool operator==(const map<Key, T, Compare, Alloc>& x,
                       const map<Key, T, Compare, Alloc>& y) {
    return x.t == y.t;
}

template <typename Key, typename T, typename Compare, typename Alloc>
inline bool operator<(const map<Key, T, Compare, Alloc>& x,
    const map<Key, T, Compare, Alloc>& y) {
    return x.t < y.t;
}

template <typename Key, typename T, typename Compare, typename Alloc>
inline void swap(map<Key, T, Compare, Alloc>& x,
                 map<Key, T, Compare, Alloc>& y) {
    x.swap(y);
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_MAP_H_

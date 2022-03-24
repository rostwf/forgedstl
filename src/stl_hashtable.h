#ifndef FORGED_STL_INTERNAL_HASHTABLE_H_
#define FORGED_STL_INTERNAL_HASHTABLE_H_

#include <algorithm>

#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_pair.h"
#include "stl_vector.h"

namespace forgedstl {

template <typename Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value val;
};

template <typename Value, typename Key, typename HashFunc,
          typename ExtractKey, typename EqualKey, typename Alloc = alloc>
class hashtable;

template <typename Value, typename Key, typename HashFunc,
          typename ExtractKey, typename EqualKey, typename Alloc = alloc>
struct __hashtable_iterator;

template <typename Value, typename Key, typename HashFunc,
          typename ExtractKey, typename EqualKey, typename Alloc = alloc>
struct __hashtable_const_iterator;

template <typename Value, typename Key, typename HashFunc,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct __hashtable_iterator {
    typedef hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
        hashtable;
    typedef __hashtable_iterator<Value, Key, HashFunc,
        ExtractKey, EqualKey, Alloc>
        iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFunc,
        ExtractKey, EqualKey, Alloc>
        const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;
    hashtable* ht;

    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) { }
    __hashtable_iterator() { }
    reference operator*() const {
        return cur->val;
    }
    pointer operator->() const {
        return &(operator*());
    }

    iterator& operator++();
    iterator operator++(int);
    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template <typename Value, typename Key, typename HashFunc,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct __hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
        hashtable;
    typedef __hashtable_iterator<Value, Key, HashFunc,
        ExtractKey, EqualKey, Alloc>
        iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFunc,
        ExtractKey, EqualKey, Alloc>
        const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;
    hashtable* ht;

    __hashtable_const_iterator(node* n, hashtable* tab)
        : cur(n), ht(tab) { }
    __hashtable_const_iterator() { }
    __hashtable_const_iterator(const iterator& it)
        : cur(it.cur), ht(it.ht) { }
    reference operator*() const {
        return cur->val;
    }
    pointer operator->() const {
        return &(operator*());
    }

    const_iterator& operator++();
    const_iterator operator++(int);
    bool operator==(const const_iterator& it) const { return cur == it.cur; }
    bool operator!=(const const_iterator& it) const { return cur != it.cur; }
};

static const int __stl_num_primes = 28;
static const unsigned long long __stl_prime_list[__stl_num_primes] = {
    53,         97,           193,         389,       769,
  1543,       3079,         6151,        12289,     24593,
  49157,      98317,        196613,      393241,    786433,
  1572869,    3145739,      6291469,     12582917,  25165843,
  50331653,   100663319,    201326611,   402653189, 805306457,
  1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long long __stl_next_prime(unsigned long long n) {
    const unsigned long long * first = __stl_prime_list;
    const unsigned long long * last = __stl_prime_list + __stl_num_primes;
    const unsigned long long * pos = std::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template <typename Value, typename Key, typename HashFunc,
    typename ExtractKey, typename EqualKey, typename Alloc>
class hashtable {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef HashFunc hasher;
    typedef EqualKey key_equal;

    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;

    typedef __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
        iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey,
        Alloc> const_iterator;

    friend struct
        __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    friend struct
        __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;

    hashtable(size_type n, const HashFunc& hf, const EqualKey& eql,
              const ExtractKey& ext)
        : hash(hf), equals(eql), get_key(ext), num_elements(0) {
        initialize_buckets(n);
    }
    hashtable(size_type n, const HashFunc& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
        initialize_buckets(n);
    }
    hashtable(const hashtable& ht)
        : hash(ht.hash), equals(ht.equals), get_key(ht.get_key()), num_elements(0) {
        copy_from(ht);
    }

    hashtable& operator=(const hashtable& ht) {
        if (&ht != this) {
            clear();
            hash = ht.hash;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(ht);
        }
    }

    ~hashtable() {
        clear();
    }

    hasher hash_funct() const {
        return hash;
    }
    key_equal key_eq() const {
        return equals;
    }

    size_type size() const {
        return num_elements;
    }
    size_type max_size() const {
        return size_type(-1);
    }
    bool empty() const {
        return size() == 0;
    }
    
    void swap(hashtable& ht) {
        std::swap(hash, ht.hash);
        std::swap(equals, ht.equals);
        std::swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        std::swap(num_elements, ht.num_elements);
    }

    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n) {
            if (buckets[n] != nullptr) {
                return iterator(buckets[n], this);
            }
        }
        return end();
    }
    iterator end() {
        return iterator(nullptr, this);
    }
    const_iterator begin() const {
        for (size_type n = 0; n < buckets.size(); ++n) {
            if (buckets[n] != nullptr) {
                return const_iterator(buckets[n], this);
            }
        }
        return end();
    }
    const_iterator end() const {
        return const_iterator(0, this);
    }

    size_type bucket_count() const {
        return buckets.size();
    }
    size_type max_buckets_count() const {
        return __stl_prime_list[__stl_num_primes - 1];
    }
    size_type elems_in_buckets(size_type bucket) const {
        size_type result = 0;
        for (node* cur = buckets[bucket]; cur != nullptr; cur = cur->next) {
            result += 1;
        }
        return result;
    }

    pair<iterator, bool> insert_unique(const value_type& obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    iterator insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    iterator insert_equal_noresize(const value_type& obj);

    template <class InputIterator>
    void insert_unique(InputIterator f, InputIterator l)
    {
        insert_unique(f, l, iterator_category(f));
    }

    template <class InputIterator>
    void insert_equal(InputIterator f, InputIterator l)
    {
        insert_equal(f, l, iterator_category(f));
    }

    template <class InputIterator>
    void insert_unique(InputIterator f, InputIterator l,
                       input_iterator_tag)
    {
        for (; f != l; ++f) {
            insert_unique(*f);
        }
    }

    template <class InputIterator>
    void insert_equal(InputIterator f, InputIterator l,
                      input_iterator_tag)
    {
        for (; f != l; ++f) {
            insert_equal(*f);
        }
    }

    template <class ForwardIterator>
    void insert_unique(ForwardIterator f, ForwardIterator l,
                       forward_iterator_tag)
    {
        size_type n = 0;
        distance(f, l, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++f) {
            insert_unique_noresize(*f);
        }
    }

    template <class ForwardIterator>
    void insert_equal(ForwardIterator f, ForwardIterator l,
                      forward_iterator_tag)
    {
        size_type n = 0;
        distance(f, l, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++f)
            insert_equal_noresize(*f);
    }

    reference find_or_insert(const value_type& obj);

    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for (first = buckets[n]; 
             first != nullptr && !equals(get_key(first->val), key);
             first = first->next) { }
        return iterator(first, this);
    }

    const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        node* first;
        for (first = buckets[n];
             first != nullptr && !equals(get_key(first->val), key);
             first = first->next) {
        }
        return const_iterator(first, this);
    }

    pair<iterator, iterator> equal_range(const key_type& key);
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

    size_type erase(const key_type& key);
    void erase(const iterator& it);
    void erase(iterator first, iterator last);

    void erase(const const_iterator& it);
    void erase(const_iterator first, const_iterator last);

    void resize(size_type num_elements_hint);
    void clear();

    friend bool operator== <> (const hashtable&, const hashtable&);

private:
    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    vector<node*, Alloc> buckets;
    size_t num_elements;

    size_type next_size(size_type n) const {
        return __stl_next_prime(n);
    }

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)nullptr);
        num_elements = 0;
    }

    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets.size());
    
    }
    size_type bkt_num(const value_type& obj) const {
        return bkt_num_key(get_key(obj));
    }
    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hash(key) % n;
    }
    size_type bkt_num(const value_type& obj, size_t n) const {
        return bkt_num_key(get_key(obj), n);
    }

    node* new_node(const value_type& obj) {
        node* n = node_allocator::allocate();
        n->next = nullptr;
        try {
            construct(&n->val, obj);
            return n;
        } catch (...) {
            node_allocator::deallocate(n);
            throw;
        }
    }

    void delete_node(node* n) {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }

    void erase_bucket(const size_type n, node* first, node* last);
    void erase_bucket(const size_type n, node* last);

    void copy_from(const hashtable& ht);
};

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
__hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&
__hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if (cur == nullptr) {
        size_type bucket = ht->bkt_num(old->val);
        while (cur == nullptr && ++bucket < ht->buckets.size()) {
            cur = ht->buckets[bucket];
        }
    }
    return *this;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
__hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
__hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&
__hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if (cur == nullptr) {
        size_type bucket = ht->bkt_num(old->val);
        while (cur == nullptr && ++bucket < ht->buckets.size()) {
            cur = ht->buckets[bucket];
        }
    }
    return *this;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
__hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++(int) {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline forward_iterator_tag
iterator_category(const __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return forward_iterator_tag();
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline Value*
value_type(const __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return (Value*)nullptr;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline typename __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::difference_type*
distance_type(const __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return (typename __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
            ::difference_type*)(nullptr);
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline forward_iterator_tag
iterator_category(const __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return forward_iterator_tag();
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline Value*
value_type(const __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return (Value*)nullptr;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline typename __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::difference_type*
distance_type(const __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>&) {
    return (typename __hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
            ::difference_type*)(nullptr);
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
bool operator==(const hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>& ht1,
                const hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>& ht2) {
    typedef typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::node node;
    if (ht1.buckets.size() != ht2.buckets.size()) {
        return false;
    }
    for (int n = 0; n < ht1.buckets.size(); ++n) {
        node* cur1 = ht1.buckets[n];
        node* cur2 = ht2.buckets[n];
        for (; cur1 != nullptr && cur2 != nullptr && cur1->val ==cur2->val;
             cur1 = cur1->next, cur2 = cur2->next) { }
        if (cur1 != nullptr || cur2 != nullptr) {
            return false;
        }
    }
    return true;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline void swap(hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>& ht1,
                 hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>& ht2) {
    ht1.swap(ht2);
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
pair<typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator, bool>
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];

    for (node* cur = first; cur != nullptr; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            return pair<iterator, bool>(iterator(cur, this), false);
        }
    }
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return pair<iterator, bool>(iterator(tmp, this), true);
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];

    for (node* cur = first; cur != nullptr; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }
    }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::reference
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::find_or_insert(const value_type& obj) {
    resize(num_elements + 1);

    size_type n = bkt_num(obj);
    node* first = buckets[n];

    for (node* cur = first; cur != nullptr; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            return cur->val;
        }
    }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return tmp->val;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
pair<typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator,
     typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator>
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::equal_range(const key_type& key) {
    typedef pair<iterator, iterator> pii;
    const size_type n = bkt_num_key(key);

    for (node* first = buckets[n]; first != nullptr; first = first->next) {
        if (equals(get_key(first->val), key)) {
            for (node* cur = first->next; cur != nullptr; cur = cur->next) {
                if (!equals(get_key(cur->val), key)) {
                    return pii(iterator(first, this), iterator(cur, this));
                }
            }
            for (size_type m = n + 1; m < buckets.size(); ++m) {
                if (buckets[m] != nullptr) {
                    return pii(iterator(first, this), iterator(buckets[m], this));
                }
            }
            return pii(iterator(first, this), end());
        }
    }
    return pii(end(), end());
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
pair<typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::const_iterator,
     typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::const_iterator>
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::equal_range(const key_type& key) const {
    typedef pair<const_iterator, const_iterator> pii;
    const size_type n = bkt_num_key(key);

    for (node* first = buckets[n]; first != nullptr; first = first->next) {
        if (equals(get_key(first->val), key)) {
            for (node* cur = first->next; cur != nullptr; cur = cur->next) {
                if (!equals(get_key(cur->val), key)) {
                    return pii(const_iterator(first, this), const_iterator(cur, this));
                }
            }
            for (size_type m = n + 1; m < buckets.size(); ++m) {
                if (buckets[m] != nullptr) {
                    return pii(const_iterator(first, this), const_iterator(buckets[m], this));
                }
            }
            return pii(const_iterator(first, this), end());
        }
    }
    return pii(end(), end());
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::size_type
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const key_type& key) {
    const size_type n = bkt_num_key(key);
    node* first = buckets[n];
    size_type erased = 0;

    if (first != nullptr) {
        node* cur = first;
        node* next = cur->next;
        while (next != nullptr) {
            if (equals(get_key(next->val), key)) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                ++erased;
                --num_elements;
            }
            else {
                cur = next;
                next = cur->next;
            }
        }
        if (equals(get_key(first->val), key)) {
            buckets[n] = first->next; // include nullptr
            delete_node(first);
            ++erased;
            --num_elements;
        }
    }
    return erased;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const iterator& it) {
    if (node* const p = it.cur) {
        const size_type n = bkt_num(p->val);
        node* cur = buckets[n];

        if (cur == p) {
            buckets[n] = cur->next;
            delte_node(cur);
            --num_elements;
        }
        else {
            node* next = cur->next;
            while (next != nullptr) {
                if (next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                }
                else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(iterator first, iterator last) {
    size_type f_bucket = first.cur == nullptr ?
        bkt_num(first.cur->val) : buckets.size();
    size_type l_bucket = last.cur == nullptr ?
        bkt_num(last.cur->val) : buckets.size();

    if (first.cur == last.cur) {
        return;
    }
    else if (f_bucket == l_bucket) {
        erase_bucket(f_bucket, first.cur, last.cur);
    }
    else {
        erase_bukcet(f_bucket, first.cur, nullptr);
        for (size_type n = f_bucket + 1; n < l_bucket; ++n) {
            erase_bucket(n, nullptr);
        }
        if (l_bucket != buckets.size()) {
            erase_bucket(l_bucket, last.cur);
        }
    }
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const_iterator first,
                                                                         const_iterator last) {
    erase(iterator(const_cast<node*>(first.cur),
                   const_cast<node*>(first.ht)),
          iterator(const_cast<node*>(last.cur),
                   const_cast<node*>(last.ht)));
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const const_iterator& it) {
    erase(iterator(const_cast<node*>(it.cur),
                   const_cast<node*>(it.ht)));
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::resize(size_type num_elements_hint) {
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) {
        const size_type n = next_size(num_elements_hint);
        if (n > old_n) {
            vector<node*, Alloc> tmp(n, (node*)nullptr);
            try {
                for (size_type bucket = 0; bucket < old_n; ++bucket) {
                    node* first = buckets[bucket];
                    while (first != nullptr) {
                        size_type new_bucket = bkt_num(first->val, n);
                        buckets[bucket] = first->next;
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);
            }
            catch (...) {
                for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                    while (tmp[bucket] != nullptr) {
                        node* next = tmp[bucket]->next;
                        delete_node(tmp[bucket]);
                        tmp[bucket] = next;
                    }
                }
                throw;
            }
        }
    }
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::clear() {
    for (size_type i = 0; i < buckets.size(); ++i) {
        node* cur = buckets[i];
        while (cur != nullptr) {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = nullptr;
    }
    num_elements = 0;
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase_bucket(const size_type n,
                                                                                node* first, node* last) {
    node* cur = buckets[n];
    if (cur == first) {
        erase_bucket(n, last);
    }
    else {
        node* next;
        for (next = cur->next; next != first; cur = next, next = cur->next) { }
        while (next != nullptr) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase_bucket(const size_type n, node* last) {
    node* cur = buckets[n];
    while (cur != last) {
        node* next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::copy_from(const hashtable& ht) {
    buckets.clear();
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), (node*)nullptr);
    try {
        for (size_type i = 0; i < ht.buckets.size(); ++i) {
            if (const node* cur = ht.buckets[i]) {
                node* copy = new_node(cur->val);
                buckets[i] = copy;

                for (node* next = cur->next; next != nullptr; cur = next, next = cur->next) {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }
            }
        }
        num_elements = ht.num_elements;
    }
    catch (...) {
        clear();
        throw;
    }
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_HASHTABLE_H_

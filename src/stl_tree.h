#ifndef FORGED_STL_INTERNAL_TREE_H_
#define FORGED_STL_INTERNAL_TREE_H_

#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_pair.h"

namespace forgedstl {

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base {
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x) {
        while (x->left != nullptr) {
            x = x->left;
        }
        return x;
    }

    static base_ptr maximum(base_ptr x) {
        while (x->right != nullptr) {
            x = x->right;
        }
        return x;
    }
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base {
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;
};

struct __rb_tree_base_iterator {
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iteraotr_category;
    typedef ptrdiff_t difference_type;
    base_ptr node;

    void increment() {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr) {
                node = node->left;
            }
        }
        else {
            base_ptr y = node->parent;
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            if (node->right != y) { // while node not points at right()
                node = y;
            }
        }
    }

    void decrement() {
        if (node->color == __rb_tree_red &&
            node->parent->parent == node) { // while node points at header
            node = node->right;
        }
        else if (node->left != nullptr) {
            base_ptr y = node->left;
            while (y->right != nullptr) {
                y = y->right;
            }
            node = y;
        }
        else {
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};

template <typename Value, typename Ref, typename Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator {
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
    typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value>* link_type;

    __rb_tree_iterator() { }
    __rb_tree_iterator(link_type x) {
        node = x;
    }
    __rb_tree_iterator(const iterator& it) {
        node = it.node;
    }

    reference operator*() const {
        return link_type(node)->value_field;
    }
    pointer operator->() {
        return &(operator*());
    }

    self& operator++() {
        increment();
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--() {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
};

inline bool operator==(const __rb_tree_base_iterator& x,
                       const __rb_tree_base_iterator& y) {
    return x.node == y.node;
}

inline bool operator!=(const __rb_tree_base_iterator& x,
                       const __rb_tree_base_iterator& y) {
    return x.node != y.node;
}

inline bidirectional_iterator_tag
iterator_category(const __rb_tree_base_iterator&) {
    return bidirectional_iterator_tag();
}

inline __rb_tree_base_iterator::difference_type*
distance_type(const __rb_tree_base_iterator&) {
    return (__rb_tree_base_iterator::difference_type*)0;
}

template <typename Value, typename Ref, typename Ptr>
inline Value* value_type(const __rb_tree_iterator<Value, Ref, Ptr>&) {
    return (Value*)0;
}

inline void
__rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base* root) {
    __rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if (x == root) {
        root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

inline void
__rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base* root) {
    __rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != 0) {
        y->right->parent = x;
    }
    y->parent = x->parent;

    if (x == root) {
        root = y;
    } 
    else if (x == x->parent->right) {
        x->parent->right = y;
    }
    else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

inline void
__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    x->color = __rb_tree_red;
    while (x != root && x->parent->color == __rb_tree_red) {
        if (x->parent == x->parent->parent->left) {
            __rb_tree_node_base* y = x->parent->parent->right;
            if (y != nullptr && y->color == __rb_tree_red) {
                x->parent->color = __rb_tree_black;
                y->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                x = x->parent->parent;
            }
            else {
                if (x == x->parent->right) {
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        else {
            __rb_tree_node_base* y = x->parent->parent->left;
            if (y != nullptr && y->color == __rb_tree_red) {
                x->parent->color = __rb_tree_black;
                y->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                x = x->parent->parent;
            }
            else {
                if (x == x->parent->left) {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = __rb_tree_black;
}

inline __rb_tree_node_base*
__rb_tree_rebalance_for_erase(__rb_tree_node_base* z,
                              __rb_tree_node_base*& root,
                              __rb_tree_node_base*& leftmost,
                              __rb_tree_node_base*& rightmost) {
    __rb_tree_node_base* y = z;
    __rb_tree_node_base* x = nullptr;
    __rb_tree_node_base* x_parent = nullptr;
    if (y->left == nullptr) { // y is z or z's successor
        x = y->right;
    } else {
        if (y->right == nullptr) {
            x = y->left;
        } else {
            y = y->right;
            while (y->left != nullptr) {
                y = y->left;
            }
            x = y->right;
        }
    }
    if (y != z) { // y is z's successor
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent;
            if (x != nullptr) { // use x to replace y
                x->parent = y->parent;
            }
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        } else { // y == z->right
            x_parent = y;
        }
        if (root == z) {                   // transplant(T, z, y) begin
            root = y;
        } else if (z->parent->left == z) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;             // transplant(T, z, y) end
        std::swap(y->color, z->color);
        y = z; // y points to nodes to be deleted
    } else { // y == z, need to deal with leftmost or rightmost
        x_parent = y->parent;
        if (x != nullptr) {
            x->parent = y->parent;
        }
        if (root == z) {                  // transplant(T, z, x) begin
            root = x;
        } else {
            if (z->parent->left = z) {
                z->parent->left = x;
            } else {
                z->parent->right = x;     // transplant(T, z, x) begin
            }
        }
        if (leftmost == z) { // z will be leftmost only when z has not left child
            if (z->right == nullptr) {
                leftmost = z->parent;
            } else {
                leftmost = __rb_tree_node_base::minimum(x);
            }
        }
        if (rightmost == z) { // z will be rightmost only when z has not right child
            if (z->left == nullptr) {
                rightmost = z->parent;
            } else {
                rightmost = __rb_tree_node_base::maximum(x);
            }
        }
    }
    if (y->color != __rb_tree_red) { // rb_delete_fixup
        while (x != root && (x == nullptr || x->color == __rb_tree_black)) { // nullptr is black
            if (x == x_parent->left) {
                __rb_tree_node_base* w = x_parent->right;
                if (w->color == __rb_tree_red) {
                    w->color = __rb_tree_black;
                    x_parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == nullptr || w->left->color == __rb_tree_black) &&
                    (w->right == nullptr || w->right->color == __rb_tree_black)) {
                    w->color = __rb_tree_black;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == nullptr || w->right->color == __rb_tree_black) {
                        if (w->left != nullptr) {
                            w->left->color = __rb_tree_red;
                        }
                        __rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __rb_tree_black;
                    if (w->right != nullptr) {
                        w->right->color = __rb_tree_black;
                    }
                    __rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else { // x == x_parent->right
                __rb_tree_node_base* w = x_parent->left;
                if (w->color == __rb_tree_red) {
                    w->color = __rb_tree_black;
                    x_parent->color = __rb_tree_red;
                    __rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == nullptr || w->right->color == __rb_tree_black) &&
                    (w->left == nullptr || w->left->color == __rb_tree_black)) {
                    w->color = __rb_tree_black;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == nullptr || w->left->color == __rb_tree_black) {
                        if (w->right != nullptr) {
                            w->right->color = __rb_tree_black;
                        }
                        w->color = __rb_tree_red;
                        __rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = __rb_tree_black;
                    if (w->left != nullptr) {
                        w->left->color = __rb_tree_black;
                    }
                    __rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x != nullptr) {
            x->color = __rb_tree_black;
        }
    }
    return y;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare,
          typename Alloc = alloc>
class rb_tree {
protected:
    typedef void* void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef __rb_tree_color_type color_type;

public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef __rb_tree_iterator<value_type, const_reference, const_pointer>
        const_iterator;

    rb_tree(const Compare& comp = Compare()) : node_count(0),
        key_compare(comp) {
        init();
    }
    rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x)
        : node_count(0), key_compare(x.key_compare) {
        header = get_node();
        color(header) = __rb_tree_red;
        if (x.root() == nullptr) {
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        } else {
            try {
                root() = __copy(x.root(), header);
            } catch (...) {
                put_node(header);
                throw;
            }
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        node_count = x.node_count;
    }
    ~rb_tree() {
        clear();
        put_node(header);
    }

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

    Compare key_comp() const {
        return key_compare;
    }
    iterator begin() {
        return leftmost();
    }
    const_iterator begin() const {
        return leftmost();
    }
    iterator end() {
        return header;
    }
    const_iterator end() const {
        return header;
    }

    bool empty() const {
        return node_count == 0;
    }
    size_type size() const {
        return node_count;
    }
    size_type max_size() const {
        return size_type(-1);
    }

    void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
        std::swap(header, t.header);
        std::swap(node_count, t.node_count);
        std::swap(key_compare, t.key_compare);
    }

    pair<iterator, bool> insert_unique(const value_type& x);
    iterator insert_equal(const value_type& x);

    iterator insert_unique(iterator position, const value_type& x);
    iterator insert_equal(iterator position, const value_type& x);

    template <typename InputIterator>
    void insert_unique(InputIterator first, InputIterator last);
    template <typename InputIterator>
    void insert_equal(InputIterator first, InputIterator last);

    void erase(iterator position);
    size_type erase(const key_type& x);
    void erase(iterator first, iterator last);

    void clear() {
        if (node_count != 0) {
            __erase(root());
            leftmost() = header;
            root() = 0;
            rightmost() = header;
            node_count = 0;
        }
    }

    iterator find(const key_type& x);
    const_iterator find(const key_type& x) const;
    size_type count(const key_type& x) const;
    iterator lower_bound(const key_type& x);
    const_iterator lower_bound(const key_type& x) const;
    iterator upper_bound(const key_type& x);
    const_iterator upper_bound(const key_type& x) const;
    pair<iterator, iterator> equal_range(const key_type& x);
    pair<const_iterator, const_iterator> equal_range(const key_type& x) const;

    bool __rb_verify() const;

protected:
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type get_node() {
        return rb_tree_node_allocator::allocate();
    }

    void put_node(link_type p) {
        rb_tree_node_allocator::deallocate(p);
    }

    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        try {
            construct(&tmp->value_field, x);
        } catch (...) {
            put_node(tmp);
            throw;
        }
        return tmp;
    }

    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }

    link_type& root() const {
        return (link_type&)header->parent;
    }
    link_type& leftmost() const {
        return (link_type&)header->left;
    }
    link_type& rightmost() const {
        return (link_type&)header->right;
    }

    // static methods
    static link_type& left(link_type x) {
        return (link_type&)(x->left);
    }
    static link_type& right(link_type x) {
        return (link_type&)(x->right);
    }
    static link_type& parent(link_type x) {
        return (link_type&)(x->parent);
    }
    static reference value(link_type x) {
        return x->value_field;
    }
    static const key_type& key(link_type x) {
        return KeyOfValue()(value(x));
    }
    static color_type& color(link_type x) {
        return (color_type&)(x->color);
    }

    static link_type& left(base_ptr x) {
        return (link_type&)(x->left);
    }
    static link_type& right(base_ptr x) {
        return (link_type&)(x->right);
    }
    static link_type& parent(base_ptr x) {
        return (link_type&)(x->parent);
    }
    static reference value(base_ptr x) {
        return ((link_type)x)->value_field;
    }
    static const key_type& key(base_ptr x) {
        return KeyOfValue()(value(link_type(x)));
    }
    static color_type& color(base_ptr x) {
        return (color_type&)(link_type(x)->color);
    }

    static link_type minimum(link_type x) {
        return (link_type)__rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x) {
        return (link_type)__rb_tree_node_base::maximum(x);
    }

private:
    iterator __insert(base_ptr x, base_ptr y, const value_type& v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init() {
        header = get_node();
        color(header) = __rb_tree_red;

        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }
};

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                 rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    x.swap(y);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
    if (this != &x) {
        clear();
        node_count = 0;
        key_compare = x.key_compare;
        if (x.root() == nullptr) {
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        } else {
            root() = __copy(x.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = x.node_count;
        }
    }
    return *this;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != nullptr) {
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);
    if (comp) {
        if (j == begin()) {
            return pair<iterator, bool>(__insert(x, y, v), true);
        }
        else {
            --j;
        }
    }
    if (key_compare(key(j.node), KeyOfValue()(v))) {
        return pair<iterator, bool>(__insert(x, y, v), true);
    }
    return pair<iterator, bool>(j, false);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
    link_type y = header;
    link_type x = root();
    while (x != nullptr) {
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    return __insert(x, y, v);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const value_type& v) {
    if (position.node == header->left) { // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node))) {
            return __insert(position.node, position.node, v);
        }
        else {
            return insert_unique(v).first;
        }
    }
    else if (position.node == header) { // end()
        if (key_compare(key(rightmost()), KeyOfValue()(v))) {
            return __insert(nullptr, rightmost(), v);
        }
        else {
            return insert_unique(v).first;
        }
    }
    else {
        iterator before = position;
        --before;
        if (key_compare(key(before.node), KeyOfValue()(v))
            && key_compare(KeyOfValue()(v), key(position.node))) {
            if (right(before.node) == 0) {
                return __insert(nullptr, before.node, v);
            }
            else {
                return __insert(position.node, position.node, v);
            }
        }
        else {
            return insert_unique(v).first;
        }
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const value_type& v) {
    if (position.node == header->left) { // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node))) {
            return __insert(position.node, position.node, v);
        }
        else {
            return inert_euqal(v);
        }
    }
    else if (position.node == header) { // end()
        if (!key_compare(KeyOfValue()(v), key(rightmost()))) {
            return __insert(nullptr, rightmost(), v);
        }
        else {
            return inert_euqal(v);
        }
    }
    else {
        iterator before = position;
        --before;
        if (!key_compare(KeyOfValue()(v), key(before.node))
            && !key_compare(key(position.node), KeyOfValue()(v))) {
            if (right(before.node) == 0) {
                return __insert(nullptr, before.node, v);
            }
            else {
                return __insert(position.node, position.node, v);
            }
        }
        else {
            return inert_euqal(v);
        }
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
template <typename InputIterator>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
        insert_unique(*first);
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
template <typename InputIterator>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
        insert_equal(*first);
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline void
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
    link_type y = (link_type)__rb_tree_rebalance_for_erase(position.node,
                                                           header->parent,
                                                           header->left,
                                                           header->right);
    destroy_node(y);
    --node_count;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const key_type& x) {
    pair<iterator, iterator> p = euqal_range(x);
    size_type n = 0;
    distance(p.first, p.second, n);
    erase(p.first, p.second);
    return n;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last) {
    if (first == begin() && last == end()) {
        clear();
    }
    else {
        while (first != last) {
            erase(first++);
        }
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const key_type& k) {
    link_type y = header;
    link_type x = root();

    while (x != nullptr) {
        if (!key_compare(key(x), k)) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const key_type& k) const {
    link_type y = header();
    link_type x = root();

    while (x != nullptr) {
        if (!key_compare(key(x), k)) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    const_iterator j = const_iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const key_type& k) const {
    pair<const_iterator, const_iterator> p = euqal_range(k);
    size_type n = 0;
    distance(p.first, p.second, n);
    return n;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const key_type& k) {
    link_type y = header;
    link_type x = root();

    while (x != nullptr) {
        if (!key_compare(key(x), k)) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    return iterator(y);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const key_type& k) const {
    link_type y = header;
    link_type x = root();

    while (x != nullptr) {
        if (!key_compare(key(x), k)) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    return const_iterator(y);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const key_type& k) {
    link_type y = header;
    link_type x = root();

    while (x != nullptr) {
        if (key_compare(k, key(x))) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    return iterator(y);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const key_type& k) const {
    link_type y = header;
    link_type x = root();

    while (x != nullptr) {
        if (key_compare(k, key(x))) {
            y = x, x = left(x);
        }
        else {
            x = right(x);
        }
    }

    return const_iterator(y);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
                 typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const key_type& k) {
    return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator,
                 typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const key_type& k) const {
    return pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
__insert(base_ptr x_, base_ptr y_, const value_type& v) {
    link_type x = (link_type)x_;
    link_type y = (link_type)y_;
    link_type z;

    if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
        z = create_node(v);
        left(y) = z;
        if (y == header) {
            root() = z;
            rightmost() = z;
        }
        else if (y == leftmost()) {
            leftmost() = z;
        }
    }
    else {
        z = create_node(v);
        right(y) = z;
        if (y == rightmost()) {
            rightmost() = z;
        }
    }
    parent(z) = y;
    left(z) = nullptr;
    right(z) = nullptr;
    __rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
    link_type top = clone_node(x);
    top->parent = p;

    try {
        if (x->right != nullptr) {
            top->right = __copy(right(x), top);
        }
        p = top;
        x = left(x);

        while (x != 0) {
            link_type y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right != nullptr) {
                y->right = __copy(right(x), y);
            }
            p = y;
            x = left(x);
        }
    } catch (...) {
        __erase(top);
    }
    return top;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x) { // without rebalancing
    while (x != nullptr) {
        __erase(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;
    }
}

inline int __black_count(__rb_tree_node_base* node, __rb_tree_node_base* root) {
    if (node == nullptr) {
        return 0;
    }
    else {
        int bc = node->color == __rb_tree_black ? 1 : 0;
        if (node == root) {
            return bc;
        }
        else {
            return bc + __black_count(node->parent, root);
        }
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
bool
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_verify() const {
    if (node_count == 0 || begin() == end()) {
        return node_count == 0 && begin() == end() &&
            header->left == header && header->right == header;
    }

    int len = __black_count(leftmost(), root());
    for (const_iterator it = begin(); it != end(); ++it) {
        link_type x = (link_type)it.node;
        link_type L = left(x);
        link_type R = right(x);

        if (x->color == __rb_tree_red) {
            if ((L != nullptr && L->color == __rb_tree_red) ||
                (R != nullptr && R->color == __rb_tree_black)) {
                return false;
            }
        }

        if (L != nullptr && key_compare(key(x), key(L))) {
            return false;
        }
        if (R != nullptr && key_compare(key(R), key(x))) {
            return false;
        }

        if (L == nullptr && R == nullptr && __black_count(x, root()) != len) {
            return false;
        }
    }

    if (leftmost() != __rb_tree_node_base::minimum(root())) {
        return false;
    }
    if (rightmost() != __rb_tree_node_base::maximum(root())) {
        return false;
    }

    return true;
}

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_TREE_H_

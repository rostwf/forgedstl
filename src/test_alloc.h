#ifndef TEST_ALLOC_H_
#define TEST_ALLOC_H_

#include <cstddef>
#include <climits>

namespace forgedstl {

template <typename T>
inline T* _allocate(ptrdiff_t size, T*) {
    std::set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)size * sizeof(T)));
    if (tmp == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <typename T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <typename T1, typename T2>
inline void _construct(T1* p, const T2& value) {
    new(p) T1(value);
}

template <typename T>
inline void _destroy(T* ptr) {
    ptr->~T();
}

template <typename T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template <typename U>
    struct rebind {
        typedef allocator<U> other;
    };

    pointer address(reference x) {
        return (pointer)&x;
    }

    const_pointer address(const_reference x) {
        return (const_pointer)&x;
    }

    const_pointer const_address(const_reference x) {
        return (const_pointer)&x;
    }

    allocator() noexcept {}
    allocator(const allocator&) noexcept = default;
    template<class U>
    allocator(const allocator<U>&) noexcept {}

    pointer allocate(size_type n, const void* hint = 0) {
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p, size_type n) {
        _deallocate(p);
    }

    size_type max_size() const {
        return size_type(UINT_MAX / sizeof(T));
    }

    void construct(pointer p, const T& value) {
        _construct(p, value);
    }

    void destroy(pointer p) {
        _destroy(p);
    }
};

} // end of namespace forgedstl

void alloc_test() {
    int ia[5] = { 0, 1, 2, 3, 4 };
    std::vector<int, forgedstl::allocator<int> > iv(ia, ia + 5);
    //std::vector<int, std::allocator<int> > iv(ia, ia + 5);

    for (size_t i = 0; i < iv.size(); ++i) {
        std::cout << iv[i] << ' ';
    }
    std::cout << std::endl;
}

#endif // TEST_ALLOC_H_

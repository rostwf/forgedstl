#ifndef FORGED_STL_INTERNAL_ALLOC_H_
#define FORGED_STL_INTERNAL_ALLOC_H_

#include <cassert>
#include <iostream>
#include <new>

#define __THROW_BAD_ALLOC std::cerr << "out of memroy" << std::endl; exit(1)

namespace forgedstl {

template <int inst>
class __malloc_alloc_template {
public:
    static void* allocate(size_t n) {
        void* result = malloc(n);
        if (result == 0) {
            result = oom_malloc(n);
        }
        return result;
    }

    static void deallocate(void* p, size_t) {
        free(p);
    }

    static void* reallocate(void* p, size_t, size_t new_sz) {
        void* result = realloc(p, new_sz);
        if (result == 0) {
            result = oom_realloc(p, new_sz);
        }
        return result;
    }

    static void(*set_malloc_handler(void(*f)())) () {
        void(*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old; // maybe (old)
    }

private:
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);
    static void(*__malloc_alloc_oom_handler)();
};

template <int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
    void(*my_malloc_handler)() = nullptr;
    void* result = nullptr;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();
        result = malloc(n);
        if (result != nullptr) {
            return result;
        }
    }
}

template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
    void(*my_malloc_handler)() = nullptr;
    void* result = nullptr;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result != nullptr) {
            return result;
        }
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

template <bool threads, int inst>
class __default_alloc_template {
public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);

private:
    enum { __ALIGN = 8 };
    enum { __MAX_BYTES = 128 };
    enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

    union obj {
        union obj* free_list_link;
        char client_data[1];
    };

    static char* start_free;
    static char* end_free;
    static size_t heap_size;

    static obj* volatile free_list[__NFREELISTS];

    static size_t ROUND_UP(size_t bytes) {
        return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);
    }
    static size_t FREELIST_INDEX(size_t bytes) {
        return (bytes + __ALIGN - 1) / __ALIGN - 1;
    }

    static void* refill(size_t size);
    static char* chunk_alloc(size_t size, int& nobjs);
};

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;
template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;
template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj*
volatile __default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::allocate(size_t size) {
    if (size > (size_t)__MAX_BYTES) {
        return malloc_alloc::allocate(size);
    }

    obj* volatile * my_free_list;
    obj* result;

    my_free_list = free_list + FREELIST_INDEX(size);
    result = *my_free_list;
    if (result == nullptr) {
        void* r = refill(ROUND_UP(size));
        return r;
    }
    *my_free_list = result->free_list_link;
    return (void*)result;
}

template <bool threads, int inst>
void __default_alloc_template<threads, inst>::deallocate(void* p, size_t size) {
    if (size > (size_t)__MAX_BYTES) {
        malloc_alloc::deallocate(p, size);
        return;
    }

    obj* q = (obj*)p;
    obj* volatile * my_free_list;

    my_free_list = free_list + FREELIST_INDEX(size);
    q->free_list_link = *my_free_list;
    *my_free_list = q;
}

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
    if (old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES) {
        return malloc_alloc::reallocate(p, old_sz, new_sz);
    }

    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) {
        return p;
    }

    void* result = allocate(new_sz);
    size_t copy_sz = old_sz > new_sz ? new_sz : old_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return result;
}

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t size) {
    int nobjs = 20;
    char* chunk = chunk_alloc(size, nobjs);
    if (nobjs == 1) {
        return chunk;
    }

    obj* volatile * my_free_list;
    obj* result;
    obj* current_obj, *next_obj;
    int i;

    my_free_list = free_list + FREELIST_INDEX(size);
    result = (obj*)chunk;
    *my_free_list = next_obj = (obj*)(chunk + size);
    for (i = 2; ; i++) {
        current_obj = next_obj;
        next_obj = (obj*)((char*)current_obj + size);
        if (i == nobjs) {
            current_obj->free_list_link = nullptr;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    /*
    i = 2;
    while (i < nobjs) {
        current_obj = next_obj;
        next_obj = (obj*)((char*)next_obj + size);
        current_obj->free_list_link = next_obj;
        ++i;
    }
    next_obj->free_list_link = nullptr;
    */
    return result;
}

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
    char* result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;

    if (bytes_left > total_bytes) {
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        nobjs = int(bytes_left / size);
        result = start_free;
        start_free += nobjs * size;
        return result;
    } else {
        // deal with the memory left
        if (bytes_left > 0) {
            obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*)start_free;
        }

        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        start_free = (char*)malloc(bytes_to_get);
        if (start_free == nullptr) {
            obj* volatile * my_free_list, *p;
            for (size_t i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (p != nullptr) {
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }
            }
            end_free = 0;
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    }
}

typedef __default_alloc_template<false, 0> alloc;

template <typename T, typename Alloc>
class simple_alloc {
public:
    static T* allocate(size_t n) {
        return n == 0 ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }
    static T* allocate() {
        return (T*)Alloc::allocate(sizeof(T));
    }
    static void deallocate(T* p, size_t n) {
        if (n != 0) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }
    static void deallocate(T* p) {
        Alloc::deallocate(p, sizeof(T));
    }
};

template <typename Alloc>
class debug_alloc {
public:
    static void*  allocate(size_t n) {
        char* result = (char*)Alloc::allocate(n + extra);
        *(size_t*)result = n;
        return result + extra;
    }

    static void deallocate(void* p, size_t n) {
        char* real_p = (char*)p - extra;
        assert(*(size_t*)real_p == n);
        Alloc::deallocate(real_p, n + extra);
    }

    static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
        char* real_p = (char*)p - extra;
        assert(*(size_t*)real_p == old_sz);
        char* result = (char*)
            Alloc::deallocate(real_p, old_sz + extra, new_sz + extra);
        *(size_t*)result = new_sz;
        return result + extra;
    }

private:
    enum { extra = 8 };
};

} // namespace forgedstl

#endif // FORGED_STL_INTERNAL_ALLOC_H_
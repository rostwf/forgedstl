#include <gtest/gtest.h>

#include "stl_alloc.h"

namespace forgedstl {

template <typename Alloc>
class AllocTest : public ::testing::Test {
protected:
    enum { extra = 8 };
    void* allocate(size_t n) {
        char* result = (char*)Alloc::allocate(n + extra);
        *(size_t*)result = n;
        return result + extra;
    }

    void deallocate(void* p, size_t n) {
        char* real_p = (char*)p - extra;
        EXPECT_EQ(n, *(size_t*)real_p);
        Alloc::deallocate(real_p, n + extra);
    }

    void* reallocate(void* p, size_t old_sz, size_t new_sz) {
        char* real_p = (char*)p - extra;
        EXPECT_EQ(old_sz, *(size_t*)real_p);
        char* result = (char*)
            Alloc::reallocate(real_p, old_sz + extra, new_sz + extra);
        *(size_t*)result = new_sz;
        return result + extra;
    }
};

using ::testing::Types;

typedef Types<__default_alloc_template<false, 0>, __malloc_alloc_template<0>> Implementations;

TYPED_TEST_CASE(AllocTest, Implementations);

TYPED_TEST(AllocTest, AllocateDeallocate) {
    for (size_t i = 0; i <= 256; ++i) {
        void* p = this->allocate(i);
        this->deallocate(p, i);
    }
}

TYPED_TEST(AllocTest, Reallocate) {
    for (int i = -128; i <= 128; ++i) {
        for (size_t j = 128; j + i <= 256; ++j) {
            void* p = this->allocate(j);
            p = this->reallocate(p, j, j + i);
            this->deallocate(p, j + i);
        }
    }
}

} // namespace forgedstl
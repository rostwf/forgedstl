#include <gtest/gtest.h>

#include "stl_uninitialized.h"

namespace forgedstl {

struct Temp {
    Temp(int x) : i(x) { }
    int i;
};
bool operator==(const Temp& x, const Temp& y) {
    return x.i == y.i;
}

TEST(UninitializedTest, FillN) {
    // POD type
    char* pc = (char*)malloc(36 * sizeof(char));
    uninitialized_fill_n(pc, 36, 'a');
    for (int i = 0; i < 36; ++i) {
        EXPECT_EQ('a', *(pc + i));
    }
    destroy(pc, pc + 36);
    free(pc);
    pc = nullptr;

    // common type
    Temp* pt = (Temp*)malloc(36 * sizeof(Temp));
    uninitialized_fill_n (pt, 36, 12);
    for (int i = 0; i < 36; ++i) {
        EXPECT_EQ(12, (pt + i)->i);
    }
}

template <typename T>
class UninitializedCopyTest : public ::testing::Test {
protected:
    template <typename InputIterator>
    void Tester(InputIterator first, InputIterator last) {
        size_t n = distance(first, last);
        T* p = (T*)malloc(n * sizeof(T));
        uninitialized_copy(first, last, p);
        InputIterator iter = first;
        for (size_t i = 0; i < n && iter != last; ++i, ++iter) {
            EXPECT_EQ(*iter, *(p + i));
        }
        destroy(p, p + n);
        free(p);
        p = nullptr;
    }
};

using ::testing::Types;
typedef Types<Temp, int, char, double> types;

TYPED_TEST_CASE(UninitializedCopyTest, types);

TYPED_TEST(UninitializedCopyTest, All) {
    int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    this->Tester(a, a + 10);
}

template <typename T>
class UninitializedFillTest : public ::testing::Test {
protected:
    template <typename T1>
    void Tester(int n, T1 value) {
        T* p = (T*)malloc(n * sizeof(T));
        uninitialized_fill(p, p + n, value);
        for (size_t i = 0; i < n; ++i) {
            EXPECT_EQ(value, *(p + i));
        }
        destroy(p, p + n);
        free(p);
        p = nullptr;
    }
};

TYPED_TEST_CASE_P(UninitializedFillTest);

TYPED_TEST_P(UninitializedFillTest, All) {
    this->Tester(20, 10);
}

REGISTER_TYPED_TEST_CASE_P(UninitializedFillTest, All);

INSTANTIATE_TYPED_TEST_CASE_P(UninitlializedFill, UninitializedFillTest, types);

} // namespace forgedstl
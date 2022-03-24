#include <gtest/gtest.h>

#include "stl_heap.h"
#include "stl_vector.h"

namespace forgedstl {

TEST(HeapTest, Test1) {
    int ia1[] = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };
    vector<int> iv(ia1, ia1 + 9);

    make_heap(iv.begin(), iv.end());
    int ia2[] = { 9, 5, 8, 3, 4, 0, 2, 3, 1 };
    ASSERT_EQ(9, iv.size());
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(ia2[i], iv[i]);
    }

    iv.push_back(7);
    push_heap(iv.begin(), iv.end());
    int ia3[] = { 9, 7, 8, 3, 5, 0, 2, 3, 1, 4 };
    ASSERT_EQ(10, iv.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(ia3[i], iv[i]);
    }

    pop_heap(iv.begin(), iv.end());
    EXPECT_EQ(9, iv.back());
    iv.pop_back();
    ASSERT_EQ(9, iv.size());
    int ia4[] = { 8, 7, 4, 3, 5, 0, 2, 3, 1 };
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(ia4[i], iv[i]);
    }

    sort_heap(iv.begin(), iv.end());
    int ia5[] = { 0, 1, 2, 3, 3, 4, 5, 7, 8 };
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(ia5[i], iv[i]);
    }
}

TEST(HeapTest, Test2) {
    int ia[] = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };
    make_heap(ia, ia + 9);
    sort_heap(ia, ia + 9);
    int ia2[] = { 0, 1, 2, 3, 3, 4, 5, 8, 9 };
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(ia2[i], ia[i]);
    }

    make_heap(ia, ia + 9);
    pop_heap(ia, ia + 9);
    EXPECT_EQ(9, ia[8]);
}

TEST(HeapTest, Test3) {
    int ia[] = { 4, 1, 7, 6, 2, 5 };
    make_heap(ia, ia + 6);
    int ia2[] = { 7, 6, 5, 1, 2, 4 };
    for (int i = 0; i < 6; ++i) {
        EXPECT_EQ(ia2[i], ia[i]);
    }
}

} // namespace forgedstl
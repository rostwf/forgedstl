#include <gtest/gtest.h>
#include <iostream>

#include "stl_function.h"
#include "stl_hashtable.h"
#include "stl_hash_fun.h"

namespace forgedstl {

TEST(HashTableTest, Basic) {
    hashtable<int, int, hash<int>, identity<int>, equal_to<int> >
        iht(50, hash<int>(), equal_to<int>());
    ASSERT_TRUE(iht.empty());
    ASSERT_EQ(0, iht.size());
    ASSERT_EQ(53, iht.bucket_count());
    ASSERT_EQ(4294967291, iht.max_buckets_count());

    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique(55);
    ASSERT_EQ(6, iht.size());

    hashtable<int, int, hash<int>, identity<int>, equal_to<int> >
        ::iterator iter = iht.begin();
    int a1[] = { 53, 55, 2, 108, 59, 63 };
    for (int i = 0; i < iht.size(); ++i, ++iter) {
        EXPECT_EQ(a1[i], *iter);
    }

    int a2[] = { 1, 3, 1, 1 };
    for (int i = 0, j = 0; i < iht.size(); ++i) {
        size_t n = iht.elems_in_buckets(i);
        if (n != 0) {
            EXPECT_EQ(a2[j++], n);
        }
    }

    for (int i = 0; i <= 47; ++i) {
        iht.insert_equal(i);
    }
    ASSERT_EQ(54, iht.size());
    ASSERT_EQ(97, iht.bucket_count());

#if 0
    for (int i = 0; i < iht.bucket_count(); ++i) {
        size_t n = iht.elems_in_buckets(i);
        if (n != 0) {
            std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
        }
    }

    iter = iht.begin();
    for (int i = 0; i < iht.size(); ++i, ++iter) {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
#endif
}

} // namespace forgedstl
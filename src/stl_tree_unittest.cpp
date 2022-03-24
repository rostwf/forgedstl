#include <gtest/gtest.h>
#include <functional>

#include "stl_function.h"
#include "stl_tree.h"

namespace forgedstl {

#if 0

TEST(RBTreeTest, Basic) {
    ASSERT_EQ(10, identity<int>()(10));

    rb_tree<int, int, identity<int>, std::less<int> > itree;
    ASSERT_TRUE(itree.empty());
    ASSERT_EQ(0, itree.size());

    itree.insert_unique(10);
    ASSERT_EQ(1, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(7);
    ASSERT_EQ(2, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(8);
    ASSERT_EQ(3, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(15);
    ASSERT_EQ(4, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(5);
    ASSERT_EQ(5, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(6);
    ASSERT_EQ(6, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(11);
    ASSERT_EQ(7, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(13);
    ASSERT_EQ(8, itree.size());
    ASSERT_TRUE(itree.__rb_verify());
    itree.insert_unique(12);

    ASSERT_FALSE(itree.empty());
    ASSERT_EQ(9, itree.size());
    rb_tree<int, int, identity<int>, std::less<int> >::iterator
        iter1 = itree.begin();
    int a[] = { 5, 6, 7, 8, 10, 11, 12, 13, 15 };
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(a[i], *iter1++);
    }

    rb_tree<int, int, identity<int>, std::less<int> > itree2(itree);
    iter1 = itree2.begin();
    rb_tree<int, int, identity<int>, std::less<int> >::iterator
        iter2 = itree2.end();
    bool b[] = { false, true, false, true, true, false, false, true, false };
    __rb_tree_base_iterator rbtiter;
    for (int i = 0; iter1 != iter2; ++iter1, ++i) {
        rbtiter = __rb_tree_base_iterator(iter1);
        EXPECT_EQ(b[i], rbtiter.node->color);
    }

    ASSERT_TRUE(itree.__rb_verify());
    ASSERT_TRUE(itree2.__rb_verify());
}

#endif

} // namespace forgedstl
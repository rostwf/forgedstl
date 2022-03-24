#include <gtest/gtest.h>

#include "stl_deque.h"

namespace forgedstl {

TEST(DequeTest, Basic) {
    deque<int> id;
    ASSERT_TRUE(id.empty());
    ASSERT_EQ(0, id.size());

    id.push_back(1);
    ASSERT_FALSE(id.empty());
    ASSERT_EQ(1, id.size());
    EXPECT_EQ(1, id[0]);
    id.push_front(2);
    ASSERT_EQ(2, id.size());
    EXPECT_EQ(2, id[0]);
    EXPECT_EQ(1, id[1]);
    id.push_back(3);
    ASSERT_EQ(3, id.size());
    EXPECT_EQ(3, id[2]);
    id.push_front(4);
    ASSERT_EQ(4, id.size());
    EXPECT_EQ(4, id[0]);

    deque<char> cd;
    ASSERT_TRUE(cd.empty());
    ASSERT_EQ(0, cd.size());
    
    cd.push_back('a');
    ASSERT_FALSE(cd.empty());
    ASSERT_EQ(1, cd.size());
    EXPECT_EQ('a', cd[0]);
    cd.push_front('b');
    ASSERT_EQ(2, cd.size());
    EXPECT_EQ('b', cd[0]);
    EXPECT_EQ('a', cd[1]);
    cd.push_back('c');
    ASSERT_EQ(3, cd.size());
    EXPECT_EQ('c', cd[2]);

    deque<double> dd;
    ASSERT_TRUE(dd.empty());
    ASSERT_EQ(0, dd.size());

    dd.push_back(1.1);
    ASSERT_FALSE(dd.empty());
    ASSERT_EQ(1, dd.size());
    EXPECT_DOUBLE_EQ(1.1, dd[0]);
    dd.push_front(20.2);
    ASSERT_EQ(2, dd.size());
    EXPECT_DOUBLE_EQ(20.2, dd[0]);
    EXPECT_DOUBLE_EQ(1.1, dd[1]);
}

TEST(DequeTest, Constructor) {
    deque<int> id1;
    ASSERT_TRUE(id1.empty());
    ASSERT_EQ(0, id1.size());

    deque<int> id2(100, 3);
    ASSERT_FALSE(id2.empty());
    ASSERT_EQ(100, id2.size());
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(3, id2[i]);
    }

    deque<int> id3(id2);
    ASSERT_FALSE(id3.empty());
    ASSERT_EQ(100, id3.size());
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(3, id3[i]);
    }

    deque<int> id4(100);
    ASSERT_FALSE(id4.empty());
    ASSERT_EQ(100, id4.size());
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(0, id4[i]);
    }

    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    deque<int> id5(a, a + 10);
    ASSERT_FALSE(id5.empty());
    ASSERT_EQ(10, id5.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, id5[i]);
    }
}

TEST(DequeTest, OperatorEqual) {
    deque<int> id1(2, 3);
    deque<int> id2(3, 2);

    id1.swap(id2);
    ASSERT_EQ(3, id1.size());
    EXPECT_EQ(2, id1[0]);
    EXPECT_EQ(2, id1[1]);
    EXPECT_EQ(2, id1[2]);
    ASSERT_EQ(2, id2.size());
    EXPECT_EQ(3, id2[0]);
    EXPECT_EQ(3, id2[1]);

    deque<int> id3;
    ASSERT_TRUE(id3.empty());
    ASSERT_EQ(0, id3.size());
    // id3 = id1;
    // ASSERT_FALSE(id3.empty());
    // ASSERT_EQ(2, id3.size());
    // EXPECT_EQ(3, id3[0]);
    // EXPECT_EQ(3, id3[1]);
}

TEST(DequeTest, Iterator) {
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    deque<int> id(a, a + 10);
    deque<int>::iterator first = id.begin();
    deque<int>::iterator last = id.end();
    ASSERT_NE(first, last);
    for (int i = 0; first < last; ++first, ++i) {
        EXPECT_EQ(i, *first);
    }
    ASSERT_EQ(first, last);
}

TEST(DequeTest, PushAndPop) {
    deque<int> id;
    id.push_back(3);
    id.push_back(4);
    ASSERT_EQ(2, id.size());
    EXPECT_EQ(3, id[0]);
    EXPECT_EQ(4, id[1]);
    id.push_front(2);
    id.push_front(1);
    ASSERT_EQ(4, id.size());
    EXPECT_EQ(1, id[0]);
    EXPECT_EQ(2, id[1]);
    EXPECT_EQ(3, id[2]);
    EXPECT_EQ(4, id[3]);

    id.pop_back();
    ASSERT_EQ(3, id.size());
    id.pop_front();
    ASSERT_EQ(2, id.size());
    EXPECT_EQ(2, id[0]);
    EXPECT_EQ(3, id[1]);
}

TEST(DequeTest, Insert) {
    deque<int> id(2, 3);
    id.insert(id.begin(), 2);
    ASSERT_EQ(3, id.size());
    EXPECT_EQ(2, id[0]);
    deque<int>::iterator iter = id.begin();
    ++iter;
    id.insert(iter, 1);
    ASSERT_EQ(4, id.size());
    EXPECT_EQ(1, id[1]);
    id.insert(iter, 2, 0);
    ASSERT_EQ(6, id.size());
    int a[] = { 2, 0, 0, 1, 3, 3 };
    for (int i = 0; i < 6; ++i) {
        EXPECT_EQ(a[i], id[i]);
    }
}

TEST(DequeTest, EraseAndResize) {
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    deque<int> id(a, a + 10);
    ASSERT_EQ(10, id.size());
    id.resize(5);
    ASSERT_EQ(5, id.size());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i, id[i]);
    }
    id.resize(7, 2);
    ASSERT_EQ(7, id.size());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i, id[i]);
    }
    EXPECT_EQ(2, id[5]);
    EXPECT_EQ(2, id[6]);

    deque<int>::iterator iter2 = id.end();
    deque<int>::iterator iter1 = iter2;
    iter1 = iter1 - 2;
    id.erase(iter1, iter2);
    ASSERT_EQ(5, id.size());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i, id[i]);
    }
    id.erase(id.end() - 1);
    ASSERT_EQ(4, id.size());
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(i, id[i]);
    }

    id.clear();
    ASSERT_EQ(0, id.size());
    ASSERT_TRUE(id.empty());
}

} // namespace forgedstl
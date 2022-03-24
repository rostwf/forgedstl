#include <gtest/gtest.h>

#include "stl_list.h"

namespace forgedstl {

TEST(ListTest, Basic) {
    list<int> il;
    ASSERT_TRUE(il.empty());
    ASSERT_EQ(0, il.size());
    il.push_back(1);
    ASSERT_FALSE(il.empty());
    ASSERT_EQ(1, il.size());
    EXPECT_EQ(1, *(il.begin()));
    il.push_front(2);
    ASSERT_EQ(2, il.size());
    EXPECT_EQ(2, il.front());
    EXPECT_EQ(1, il.back());


    list<char> cl;
    ASSERT_TRUE(cl.empty());
    ASSERT_EQ(0, cl.size());
    cl.push_back('h');
    ASSERT_FALSE(cl.empty());
    ASSERT_EQ(1, cl.size());
    EXPECT_EQ('h', *(cl.begin()));
    cl.push_front('i');
    ASSERT_EQ(2, cl.size());
    EXPECT_EQ('i', cl.front());
    EXPECT_EQ('h', cl.back());

    list<double> dl;
    ASSERT_TRUE(dl.empty());
    ASSERT_EQ(0, dl.size());
    dl.push_back(20.3);
    dl.push_back(10.2);
    dl.push_front(200.22);
    ASSERT_FALSE(dl.empty());
    ASSERT_EQ(3, dl.size());
    EXPECT_DOUBLE_EQ(200.22, dl.front());
    EXPECT_DOUBLE_EQ(10.2, dl.back());
}

TEST(ListTest, Iterator) {
    list<int> il;
    ASSERT_EQ(il.begin(), il.end());
    il.push_back(1);
    il.push_back(2);
    il.push_back(3);
    il.push_back(4);
    il.push_back(5);
    list<int>::iterator iter = il.begin();
    ASSERT_NE(iter, il.end());
    for (int i = 1; iter != il.end(); ++iter, ++i) {
        EXPECT_EQ(i, *iter);
    }
}

TEST(ListTest, Construct) {
    list<int> il1;
    ASSERT_EQ(il1.begin(), il1.end());
    ASSERT_TRUE(il1.empty());
    ASSERT_EQ(0, il1.size());

    list<int> il2(10);
    ASSERT_NE(il2.begin(), il2.end());
    ASSERT_EQ(10, il2.size());
    {
        list<int>::iterator iter2 = il2.begin();
        for (int i = 0; i < 10; ++i, ++iter2) {
            EXPECT_EQ(0, *iter2);
        }
    }

    list<int> il3(10, 3);
    ASSERT_EQ(10, il3.size());
    {
        list<int>::iterator iter3 = il3.begin();
        for (int i = 0; i < 10; ++i, ++iter3) {
            EXPECT_EQ(3, *iter3);
        }
    }

    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    list<int> il4(a, a + 10);
    ASSERT_EQ(10, il4.size());
    {
        list<int>::iterator iter4 = il4.begin();
        for (int i = 0; i < 10; ++i, ++iter4) {
            EXPECT_EQ(i, *iter4);
        }
    }
}

TEST(ListTest, OperatorEqual) {
    list<int> il1(2, 3);
    list<int> il2(2, 3);

    EXPECT_EQ(il1, il2);
}

TEST(ListTest, Insert) {
    list<int> il;
    il.insert(il.begin());
    ASSERT_EQ(1, il.size());
    EXPECT_EQ(0, *(il.begin()));

    il.insert(il.begin(), 1);
    ASSERT_EQ(2, il.size());
    EXPECT_EQ(1, il.front());
    EXPECT_EQ(0, il.back());

    il.insert(il.begin(), 2, 2);
    ASSERT_EQ(4, il.size());
    int a1[] = { 2, 2, 1, 0 };
    list<int>::iterator iter = il.begin();
    for (int i = 0; i < 4; ++i, ++iter) {
        EXPECT_EQ(a1[i], *iter);
    }

    il.insert(il.begin(), a1, a1 + 4);
    ASSERT_EQ(8, il.size());
    int a2[] = { 2, 2, 1, 0, 2, 2, 1, 0 };
    iter = il.begin();
    for (int i = 0; i < 8; ++i, ++iter) {
        EXPECT_EQ(a2[i], *iter);
    }
}

TEST(ListTest, Reduce) {
    int a[] = { 0, 1, 2, 3, 4, 5, 6 };
    list<int> il(a, a + 7);

    il.pop_back();
    ASSERT_EQ(6, il.size());
    list<int>::iterator iter = il.begin();
    for (int i = 0; i < 6; ++i, ++iter) {
        EXPECT_EQ(i, *iter);
    }
    il.pop_front();
    ASSERT_EQ(5, il.size());
    iter = il.begin();
    for (int i = 1; i < 6; ++i, ++iter) {
        EXPECT_EQ(i, *iter);
    }

    il.erase(il.begin());
    ASSERT_EQ(4, il.size());
    iter = il.begin();
    for (int i = 2; i < 6; ++i, ++iter) {
        EXPECT_EQ(i, *iter);
    }
    iter = il.begin();
    ++iter;
    ++iter;
    il.erase(il.begin(), iter);
    ASSERT_EQ(2, il.size());
    iter = il.begin();
    EXPECT_EQ(4, *iter++);
    EXPECT_EQ(5, *iter);

    il.resize(1);
    ASSERT_EQ(1, il.size());
    EXPECT_EQ(4, il.front());

    il.resize(3, 2);
    ASSERT_EQ(3, il.size());
    iter = il.begin();
    EXPECT_EQ(4, *iter++);
    EXPECT_EQ(2, *iter++);
    EXPECT_EQ(2, *iter);

    il.clear();
    ASSERT_TRUE(il.empty());
    ASSERT_EQ(il.begin(), il.end());
}

TEST(ListTest, Splice) {
    list<int> il1(2, 3);
    list<int> il2(3, 4);

    il1.splice(il1.begin(), il2, il2.begin());
    ASSERT_EQ(3, il1.size());
    ASSERT_EQ(2, il2.size());
    list<int>::iterator iter = il1.begin();
    EXPECT_EQ(4, *iter++);
    EXPECT_EQ(3, *iter++);
    EXPECT_EQ(3, *iter++);

    il1.splice(il1.begin(), il2, il2.begin(), il2.end());
    ASSERT_EQ(5, il1.size());
    ASSERT_EQ(0, il2.size());
    int a1[] = { 4, 4, 4, 3, 3 };
    iter = il1.begin();
    for (int i = 0; i < 5; ++i, ++iter) {
        EXPECT_EQ(a1[i], *iter);
    }

    list<int> il3(2, 2);
    il1.splice(il1.end(), il3);
    ASSERT_EQ(7, il1.size());
    int a2[] = { 4, 4, 4, 3, 3, 2, 2 };
    iter = il1.begin();
    for (int i = 0; i < 7; ++i, ++iter) {
        EXPECT_EQ(a2[i], *iter);
    }
}

TEST(ListTest, Algo) {
    int a1[] = { 0, 1, 2, 2, 4, 4, 6, 6, 7 };
    int a2[] = { 0, 1, 2, 2, 4, 4, 7 };
    list<int> il1(a1, a1 + 9);
    il1.remove(6);
    ASSERT_EQ(7, il1.size());
    list<int>::iterator iter1 = il1.begin();
    for (int i = 0; i < 7; ++i, ++iter1) {
        EXPECT_EQ(a2[i], *iter1);
    }
    il1.unique();
    int a3[] = { 0, 1, 2, 4, 7 };
    ASSERT_EQ(5, il1.size());
    iter1 = il1.begin();
    for (int i = 0; i < 5; ++i, ++iter1) {
        EXPECT_EQ(a3[i], *iter1);
    }

    int a4[] = { 0, 2, 4, 6, 8 };
    int a5[] = { 1, 3, 5, 7, 9 };
    list<int> il2(a4, a4 + 5);
    list<int> il3(a5, a5 + 5);
    il2.merge(il3);
    int a6[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    list<int>::iterator iter2 = il2.begin();
    for (int i = 0; i < 10; ++i, ++iter2) {
        EXPECT_EQ(a6[i], *iter2);
    }

    int a7[] = { 9, 3, 2, 6, 7, 1, 5, 4, 8, 0 };
    list<int> il4(a7, a7 + 10);
    il4.sort();
    list<int>::iterator iter4 = il4.begin();
    for (int i = 0; i < 10; ++i, ++iter4) {
        EXPECT_EQ(a6[i], *iter4);
    }
    il4.reverse();
    iter4 = il4.begin();
    for (int i = 9; i >= 0; --i, ++iter4) {
        EXPECT_EQ(a6[i], *iter4);
    }
}

} // namespace forgedstl
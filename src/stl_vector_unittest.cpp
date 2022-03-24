#include <gtest/gtest.h>

#include "stl_vector.h"

namespace forgedstl {

TEST(VectorTest, Basic) {
    vector<float> fv;
    fv.push_back(float(32.1));
    fv.push_back(float(40.3));
    ASSERT_EQ(2, fv.size());
    EXPECT_FLOAT_EQ(float(32.1), fv[0]);
    EXPECT_FLOAT_EQ(float(40.3), fv[1]);

    vector<double> dv;
    dv.push_back(32.1);
    dv.push_back(40.3);
    ASSERT_EQ(2, dv.size());
    EXPECT_DOUBLE_EQ(32.1, dv[0]);
    EXPECT_DOUBLE_EQ(40.3, dv[1]);

    vector<char> cv;
    cv.push_back('h');
    cv.push_back('v');
    ASSERT_EQ(2, cv.size());
    EXPECT_EQ('h', cv[0]);
    EXPECT_EQ('v', cv[1]);

    vector<vector<double> > dvv;
    vector<double> dv2(2, 12.3);
    dvv.push_back(dv);
    dvv.push_back(dv2);
    ASSERT_EQ(2, dvv.size());
    EXPECT_DOUBLE_EQ(32.1, dvv[0][0]);
    EXPECT_DOUBLE_EQ(40.3, dvv[0][1]);
    EXPECT_DOUBLE_EQ(12.3, dvv[1][0]);
    EXPECT_DOUBLE_EQ(12.3, dvv[1][1]);
}

TEST(VectorTest, SizeAndCapacity) {
    vector<int> iv1;
    ASSERT_TRUE(iv1.empty());
    EXPECT_EQ(0, iv1.size());
    EXPECT_EQ(0, iv1.capacity());

    iv1.push_back(0);
    ASSERT_FALSE(iv1.empty());
    EXPECT_EQ(1, iv1.size());
    EXPECT_EQ(1, iv1.capacity());

    iv1.push_back(1);
    EXPECT_EQ(2, iv1.size());
    EXPECT_EQ(2, iv1.capacity());

    iv1.push_back(2);
    EXPECT_EQ(3, iv1.size());
    EXPECT_EQ(4, iv1.capacity());

    iv1.push_back(3);
    EXPECT_EQ(4, iv1.size());
    EXPECT_EQ(4, iv1.capacity());

    iv1.push_back(4);
    EXPECT_EQ(5, iv1.size());
    EXPECT_EQ(8, iv1.capacity());

    iv1.reserve(10);
    EXPECT_EQ(5, iv1.size());
    EXPECT_EQ(10, iv1.capacity());

    iv1.reserve(5);
    EXPECT_EQ(5, iv1.size());
    EXPECT_EQ(10, iv1.capacity());

    iv1.pop_back();
    EXPECT_EQ(4, iv1.size());
    EXPECT_EQ(10, iv1.capacity());
}

TEST(VectorTest, Construct) {
    vector<int> iv1(2, 3);
    ASSERT_EQ(2, iv1.size());
    EXPECT_EQ(2, iv1.capacity());
    EXPECT_EQ(3, iv1[0]);
    EXPECT_EQ(3, iv1[1]);

    vector<int> iv2(iv1);
    ASSERT_EQ(2, iv2.size());
    EXPECT_EQ(2, iv2.capacity());
    EXPECT_EQ(3, iv2[0]);
    EXPECT_EQ(3, iv2[1]);
    EXPECT_EQ(iv1, iv2);

    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vector<int> iv3(a, a + 10);
    ASSERT_EQ(10, iv3.size());
    EXPECT_EQ(10, iv3.capacity());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, iv3[i]);
    }

    vector<int> iv4(10);
    ASSERT_EQ(10, iv4.size());
    EXPECT_EQ(10, iv4.capacity());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(0, iv4[i]);
    }
}

TEST(VectorTest, Iterator) {
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vector<int> iv(a, a + 10);
    vector<int>::iterator iter = iv.begin();
    vector<int>::iterator last = iv.end();
    ASSERT_NE(iter, last);
    ASSERT_LT(iter, last);

    for (int i = 0; iter != last; ++iter, ++i) {
        EXPECT_EQ(i, *iter);
    }
    ASSERT_EQ(iter, last);
}

TEST(VectorTest, Swap) {
    vector<int> iv1(2, 3);
    vector<int> iv2(3, 4);
    iv1.swap(iv2);
    ASSERT_EQ(3, iv1.size());
    EXPECT_EQ(3, iv1.capacity());
    EXPECT_EQ(4, iv1[0]);
    EXPECT_EQ(4, iv1[1]);
    EXPECT_EQ(4, iv1[2]);
}

TEST(VectorTest, FrontAndBack) {
    int a[] = { 0, 1, 2, 3, 4, 5 };
    vector<int> iv1(a, a + 6);
    EXPECT_EQ(0, iv1.front());
    EXPECT_EQ(5, iv1.back());
}

TEST(VectorTest, Insert) {
    vector<int> iv;
    ASSERT_EQ(0, iv.size());

    iv.insert(iv.begin());
    ASSERT_EQ(1, iv.size());
    EXPECT_EQ(0, iv[0]);

    iv.insert(iv.begin(), 1);
    ASSERT_EQ(2, iv.size());
    EXPECT_EQ(1, iv[0]);

    iv.insert(iv.end(), 2);
    ASSERT_EQ(3, iv.size());
    EXPECT_EQ(2, iv[2]);

    iv.insert(iv.begin() + 1, 1);
    ASSERT_EQ(4, iv.size());
    EXPECT_EQ(1, *(iv.begin() + 1));

    int a1[] = { 1, 1, 0, 2 };
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(a1[i], iv[i]);
    }

    int a2[] = { 2, 3, 4 };
    iv.insert(iv.begin(), a2, a2 + 3);
    int a3[] = { 2, 3, 4, 1, 1, 0, 2 };
    for (int i = 0; i < 7; ++i) {
        EXPECT_EQ(a3[i], iv[i]);
    }

    iv.insert(iv.begin(), 2, 100);
    int a4[] = { 100, 100, 2, 3, 4, 1, 1, 0, 2 };
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(a4[i], iv[i]);
    }
}

TEST(VectorTest, Reduce) {
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vector<int> iv(a, a + 10);
    ASSERT_EQ(10, iv.size());

    iv.pop_back();
    ASSERT_EQ(9, iv.size());
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(a[i], iv[i]);
    }
    iv.pop_back();
    ASSERT_EQ(8, iv.size());
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(a[i], iv[i]);
    }

    iv.erase(iv.end() - 1);
    ASSERT_EQ(7, iv.size());
    for (int i = 0; i < 7; ++i) {
        EXPECT_EQ(a[i], iv[i]);
    }

    iv.erase(iv.begin(), iv.begin() + 3);
    ASSERT_EQ(4, iv.size());
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(a[i + 3], iv[i]);
    }

    iv.resize(2);
    ASSERT_EQ(2, iv.size());
    EXPECT_EQ(3, iv[0]);
    EXPECT_EQ(4, iv[1]);

    iv.resize(3, 5);
    ASSERT_EQ(3, iv.size());
    EXPECT_EQ(3, iv[0]);
    EXPECT_EQ(4, iv[1]);
    EXPECT_EQ(5, iv[2]);

    iv.clear();
    ASSERT_EQ(0, iv.size());
    ASSERT_TRUE(iv.empty());
}

} // namespace forgedstl
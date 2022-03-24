#include <gtest/gtest.h>

#include "stl_stack.h"

namespace forgedstl {

TEST(StackTest, Test) {
    stack<int> is;
    ASSERT_TRUE(is.empty());
    ASSERT_EQ(0, is.size());
    is.push(0);
    ASSERT_FALSE(is.empty());
    ASSERT_EQ(1, is.size());
    is.push(1);
    ASSERT_EQ(2, is.size());
    EXPECT_EQ(1, is.top());
    is.push(2);
    EXPECT_EQ(2, is.top());
    is.push(100);
    EXPECT_EQ(100, is.top());
    is.pop();
    EXPECT_EQ(2, is.top());
    is.pop();
    EXPECT_EQ(1, is.top());
    is.pop();
    EXPECT_EQ(0, is.top());
    is.pop();
    EXPECT_EQ(0, is.size());
    ASSERT_TRUE(is.empty());
}

} // namespace forgedstl
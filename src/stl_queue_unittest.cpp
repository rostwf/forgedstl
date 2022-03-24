#include <gtest/gtest.h>

#include "stl_queue.h"

namespace forgedstl {

TEST(QueueTest, Queue) {
    queue<int> iq;
    ASSERT_TRUE(iq.empty());
    ASSERT_EQ(0, iq.size());
    iq.push(1);
    EXPECT_EQ(1, iq.front());
    iq.push(2);
    EXPECT_EQ(1, iq.front());
    iq.pop();
    ASSERT_EQ(1, iq.size());
    EXPECT_EQ(2, iq.front());
    iq.push(3);
    EXPECT_EQ(3, iq.back());
    iq.pop();
    EXPECT_EQ(3, iq.front());
    iq.pop();
    ASSERT_TRUE(iq.empty());
}

TEST(QueueTest, PriorityQueue) {
    int ia[] = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };
    priority_queue<int> ipq(ia, ia + 9);
    ASSERT_EQ(9, ipq.size());
    EXPECT_EQ(9, ipq.top());

    ipq.push(10);
    ipq.push(3);
    ASSERT_EQ(11, ipq.size());
    int ia2[] = { 10, 9, 8, 5, 4, 3, 3, 3, 2, 1, 0 };
    for (int i = 0; i < 11; ++i) {
        EXPECT_EQ(ia2[i], ipq.top());
        ipq.pop();
    }
}

} // namespace forgedstl
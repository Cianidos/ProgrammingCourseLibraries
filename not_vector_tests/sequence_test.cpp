#include "pch.h"
#include "sequence.h"

template<is_linear_space<int> T>
T add_two(T a, T b)
{
    return a + b;
}

TEST(linear_space, TestName) {
    EXPECT_EQ(add_two(1, 2), 3);
}
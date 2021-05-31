#include "pch.h"

#include <numeric>

#include "dynamic_array.h"

using namespace std;
using namespace collections;

TEST(dynamic_array, default_constructror) 
{
    dynamic_array<int> a;
    EXPECT_EQ(a.size(), 0);
    EXPECT_ANY_THROW(a[0]);
    EXPECT_EQ(a.begin(), a.end());
    EXPECT_EQ(a.cbegin(), a.cend());
}

TEST(dynamic_array, initialization_list) 
{
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(a[i], i);
        }
    }
    {
        dynamic_array a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(a[i], pair(i, 0));
        }

        dynamic_array<pair<int,int>> 
        b{ {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0} };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(b[i], pair(i, 0));
            EXPECT_EQ(b[i], a[i]);
        }
    }
}

TEST(dynamic_array, copying) 
{
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        dynamic_array b(a);
        dynamic_array c = b;

        EXPECT_EQ(a, b);
        EXPECT_EQ(c, a);
    }
    {
        dynamic_array a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        dynamic_array b(a);
        dynamic_array c = b;

        EXPECT_EQ(a, b);
        EXPECT_EQ(c, a);
    }
}

TEST(dynamic_array, moving)
{
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        dynamic_array b(a);
        dynamic_array c(std::move(b));

        EXPECT_EQ(a, c);
        EXPECT_EQ(b.size(), 0);
        EXPECT_EQ(b.begin(), b.end());
        EXPECT_EQ(b.cbegin(), b.cend());
        EXPECT_ANY_THROW(b[0]);

        dynamic_array d = std::move(c);

        EXPECT_EQ(a, d);
        EXPECT_EQ(c.size(), 0);
        EXPECT_EQ(c.begin(), c.end());
        EXPECT_EQ(c.cbegin(), c.cend());
        EXPECT_ANY_THROW(c[0]);
    }
    {
        dynamic_array a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        dynamic_array b(a);
        dynamic_array c(std::move(b));

        EXPECT_EQ(a, c);
        EXPECT_EQ(b.size(), 0);
        EXPECT_EQ(b.begin(), b.end());
        EXPECT_EQ(b.cbegin(), b.cend());
        EXPECT_ANY_THROW(b[0]);

        dynamic_array d = std::move(c);

        EXPECT_EQ(a, d);
        EXPECT_EQ(c.size(), 0);
        EXPECT_EQ(c.begin(), c.end());
        EXPECT_EQ(c.cbegin(), c.cend());
        EXPECT_ANY_THROW(c[0]);
    }
}

TEST(dynamic_array, comparision) 
{
    dynamic_array<int>
        a{ 1, 2, 3, 4, 5 },
        b{ 10, 1 },
        c{ 1, 2, 3 },
        d{ 1, 2, 3, 4, 5 };

    EXPECT_EQ(a, d);
    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_GT(b, a);
    EXPECT_LT(c, a);
}

TEST(dynamic_array, iterators)
{
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        auto b = begin(a);
        auto e = end(a);

        int ii = 0;
        for (auto i = b; i != e; ++i)
        {
            EXPECT_EQ(*i, a[ii]);
            ++ii;
        }
    }
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        auto b = begin(a);
        auto e = end(a);

        int ii = 0;
        for (auto i = b; i != e; ++i)
        {
            EXPECT_EQ(*i, a[ii]);
            ++ii;
        }
    }
    {
        dynamic_array a{ 0, 1, 2, 3, 4, 5 };
        auto b = rbegin(a);
        auto e = rend(a);

        int ii = size(a) - 1;
        for (auto i = b; i != e; ++i)
        {
            EXPECT_EQ(*i, a[ii]);
            --ii;
        }
    }
}

TEST(dynamic_array, iterator_constructing)
{
    std::vector a{1,2,3,4,5,};
    dynamic_array<int> b(begin(a), end(a));

    for (size_t i = 0; i < size(a); ++i)
    {
        EXPECT_EQ(a[i], b[i]);
    }


    dynamic_array<int> c(b.begin(), b.end());
    for (size_t i = 0; i < size(a); ++i)
    {
        EXPECT_EQ(c[i], b[i]);
    }
}



constexpr int count_const_sum(const int n)
{
    dynamic_array<int> buf(n, 0);
    std::iota(begin(buf), end(buf), 1);
    return std::accumulate(buf.begin(), buf.end(), 0);
}

constexpr int count_smart_sum(int n)
{
    return (n * (1 + n)) / 2;
}

TEST(dynamic_array, constexpr_context)
{
    constexpr int n = 10000;
    constexpr int a = count_const_sum(n);
    constexpr int b = count_smart_sum(n);

    EXPECT_EQ(a, b);
}

TEST(dynamic_array, comparision2) {
    dynamic_array<int>
        a{ 1, 2, 3, 4, 5 },
        b{ 10, 1 },
        c{ 1, 2, 3 },
        d{ 1, 2, 3, 4, 5 };

    EXPECT_EQ(a, d);
    EXPECT_GT(b, a);
    EXPECT_LT(c, a);
}

TEST(dynamic_array, concepts)
{
    EXPECT_TRUE(std::ranges::range<dynamic_array<int>>);
}


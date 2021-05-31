#include "pch.h"

#include <linked_list.h>
#include <numeric>
#include <ranges>
#include <algorithm>

using namespace std;
using namespace collections;

TEST(linked_list, default_constructror) 
{
    linked_list<int> a;
    EXPECT_EQ(a.size(), 0);
    EXPECT_ANY_THROW(a[0]);
    EXPECT_EQ(a.begin(), a.end());
    EXPECT_EQ(a.cbegin(), a.cend());
}

TEST(linked_list, initialization_list) 
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(a[i], i);
        }
    }
    {
        linked_list a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(a[i], pair(i, 0));
        }

        linked_list<pair<int,int>> 
        b{ {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0} };
        for (int i = 0; i < 6; ++i)
        {
            EXPECT_EQ(b[i], pair(i, 0));
            EXPECT_EQ(b[i], a[i]);
        }
    }
}

TEST(linked_list, copying) 
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        linked_list b(a);
        linked_list c = b;

        EXPECT_EQ(a, b);
        EXPECT_EQ(c, a);
    }
    {
        linked_list a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        linked_list b(a);
        linked_list c = b;

        EXPECT_EQ(a, b);
        EXPECT_EQ(c, a);
    }
}

TEST(linked_list, moving)
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        linked_list b(a);
        linked_list c(std::move(b));

        EXPECT_EQ(a, c);
        EXPECT_EQ(b.size(), 0);
        EXPECT_EQ(b.begin(), b.end());
        EXPECT_EQ(b.cbegin(), b.cend());
        EXPECT_ANY_THROW(b[0]);

        linked_list d = std::move(c);

        EXPECT_EQ(a, d);
        EXPECT_EQ(c.size(), 0);
        EXPECT_EQ(c.begin(), c.end());
        EXPECT_EQ(c.cbegin(), c.cend());
        EXPECT_ANY_THROW(c[0]);
    }
    {
        linked_list a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        linked_list b(a);
        linked_list c(std::move(b));

        EXPECT_EQ(a, c);
        EXPECT_EQ(b.size(), 0);
        EXPECT_EQ(b.begin(), b.end());
        EXPECT_EQ(b.cbegin(), b.cend());
        EXPECT_ANY_THROW(b[0]);

        linked_list d = std::move(c);

        EXPECT_EQ(a, d);
        EXPECT_EQ(c.size(), 0);
        EXPECT_EQ(c.begin(), c.end());
        EXPECT_EQ(c.cbegin(), c.cend());
        EXPECT_ANY_THROW(c[0]);
    }
}

TEST(linked_list, comparision) 
{
    linked_list<int>
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

TEST(linked_list, rb_for_emplace)
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        linked_list<int> b;
        linked_list<int> c;
        linked_list<int> d;

        for (auto i : a)
        {
            b.push_back(i);
        }
        for (const auto i : a)
        {
            c.push_back(i);
        }
        for (const auto& i : a)
        {
            d.push_back(i);
        }
        EXPECT_EQ(a, b);
        EXPECT_EQ(a, c);
        EXPECT_EQ(a, d);
    }
    {
        linked_list a{ pair(0, 0), pair(1, 0), pair(2, 0), pair(3, 0), pair(4, 0), pair(5, 0) };
        linked_list<pair<int, int>> b;

        linked_list<pair<int, int>> c;
        linked_list<pair<int, int>> d;

        for (auto i : a)
        {
            b.push_back(i);
        }
        for (const auto i : a)
        {
            c.push_back(i);
        }
        for (const auto& i : a)
        {
            d.push_back(i);
        }
        EXPECT_EQ(a, b);
        EXPECT_EQ(a, c);
        EXPECT_EQ(a, d);
    }
}

TEST(linked_list, iterators)
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
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
        linked_list a{ 0, 1, 2, 3, 4, 5 };
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
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        auto b = rbegin(a);
        auto e = rend(a);

        size_t ii = size(a) - 1;
        for (auto i = b; i != e; ++i)
        {
            EXPECT_EQ(*i, a[ii]);
            --ii;
        }
    }
}

TEST(linked_list, iterator_constructing)
{
    std::vector a{1,2,3,4,5,};
    linked_list<int> b(begin(a), end(a));

    for (size_t i = 0; i < size(a); ++i)
    {
        EXPECT_EQ(a[i], b[i]);
    }


    linked_list<int> c(b.begin(), b.end());
    for (size_t i = 0; i < size(a); ++i)
    {
        EXPECT_EQ(c[i], b[i]);
    }
}


TEST(linked_list, push_front)
{
    {
        linked_list a{ 0, 1, 2, 3, 4, 5 };
        linked_list<int> b;
        linked_list<int> c(a);
        linked_list<int> d(a);

        for (auto i : a)
        {
            b.push_front(i);
        }
        {
            auto first = begin(d);
            auto last = end(d);
            while ((first != last) && (first != --last)) {
                std::iter_swap(first++, last);
            }
        }
        std::reverse(begin(c), end(c));
        EXPECT_EQ(d, b);
        EXPECT_EQ(c, b);
    }
}

constexpr int count_const_sum(const int n)
{
    linked_list<int> buf(n, 0);
    iota(begin(buf), end(buf), 1);
    return accumulate(buf.begin(), buf.end(), 0);
}
constexpr int count_smart_sum(int n)
{
    return (n * (1 + n)) / 2;
}

TEST(linked_list, constexpr_context)
{
    constexpr int n = 10000;
    int a = count_const_sum(n);
    constexpr int b = count_smart_sum(n);

    EXPECT_EQ(a, b);
}
TEST(linked_list, emplace)
{
    linked_list<int> a;
    int e = 10;
    for (int i = 0; i < e; ++i)
    {
        a.emplace_back(i);
    }
    for (int i = 0; i < e; ++i)
    {
        EXPECT_EQ(i, a[i]);
    }
}

TEST(linked_list, emplace2)
{
    linked_list<std::pair<int, double>> a;
    int e = 10;
    for (int i = 0; i < e; ++i)
    {
        a.emplace_back(i, static_cast<double>(i));
    }
    for (int i = 0; i < e; ++i)
    {
        EXPECT_EQ(std::make_pair(i, static_cast<double>(i)), a[i]);
    }
}

TEST(linked_list, emplace3)
{
    linked_list<std::pair<int, double>> a;
    int e = 10;
    for (int i = 0; i < e; ++i)
    {
        a.emplace_back(i, static_cast<double>(i));
    }
    for (auto [fst, snd] : a)
    {
        EXPECT_EQ(static_cast<double>(fst), snd);
    }
}

TEST(linked_list, comparision2) {
    linked_list<int>
        a{ 1, 2, 3, 4, 5 },
        b{ 10, 1 },
        c{ 1, 2, 3 },
        d{ 1, 2, 3, 4, 5 };

    EXPECT_EQ(a, d);
    EXPECT_GT(b, a);
    EXPECT_LT(c, a);
}

TEST(linked_list, concepts)
{
    EXPECT_TRUE(std::ranges::range<linked_list<int>>);
}


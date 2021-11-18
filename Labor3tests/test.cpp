#include <gtest/gtest.h>

#include <Nodes.hpp>

using namespace std;

#include <ctime>
#include <functional>
#include <random>

#include "testing.h"
using namespace testing;

//std::uniform_int_distribution<> range;
//std::mt19937 gen(time(nullptr));
//auto random = bind(range, gen);
//
//template <typename T>
//struct random_generator_impl
//{
//    T operator()()
//    {
//        return static_cast<T>(random());
//    }
//};
//
//
//template <typename T, typename U>
//struct random_generator_impl<std::pair<T, U>> {
//    std::pair<T, U> operator()()
//    {
//        return std::pair{ random_generator<T>(), random_generator<T>() };
//    }
//};


/*
template<typename IteratorLhs, typename IteratorRhs>
constexpr bool is_equal_collections
(
    IteratorLhs begin_it_lhs, IteratorLhs end_it_lhs,
    IteratorRhs begin_it_rhs, IteratorRhs end_it_rhs
)
{
    const size_t len_1 = std::distance(begin_it_lhs, end_it_lhs);
    const size_t len_2 = std::distance(begin_it_rhs, end_it_rhs);

    if (len_1 != len_2)
    {
        return false;
    }

    for (; begin_it_lhs != end_it_lhs;
        ++begin_it_lhs, ++begin_it_rhs)
    {
        if (*(begin_it_lhs) != *(begin_it_rhs))
        {
            return false;
        }
    }
    return true;
}
*/

template<typename RangeLhs, typename RangeRhs>
constexpr bool is_equal_collections(const RangeLhs& lhs, const RangeRhs& rhs)
{
    auto begin_it_lhs = std::begin(lhs),
        end_it_lhs = std::end(lhs);
    auto begin_it_rhs = std::begin(rhs),
        end_it_rhs = std::end(rhs);
    return is_equal_collections(begin_it_lhs, end_it_lhs, begin_it_rhs, end_it_rhs);
}

TEST(BinaryTree, sorting)
{
    std::vector<int> data(10000);
    std::generate(data.begin(), data.end(), random_generator<int>{});

    {
        std::set correct(data.begin(), data.end());
        std::set<int, std::greater<>> correct_reverse(data.begin(),data.end());

        BinaryTree t(data.begin(), data.end());

        EXPECT_EQ(correct.size(), t.size());
        auto it_set = correct.begin(),
            it_set_e = correct.end();
        auto it_bt = t.begin(),
            it_bt_e = t.end();


        for(;it_set != it_set_e && it_bt != it_bt_e; ++it_bt, ++it_set)
        {
            EXPECT_EQ(*it_bt, *it_set);
        }

        auto it_setr = correct_reverse.begin(),
            it_setr_e = correct_reverse.end();
        t.set_pattern({1, -1, 0});

        for(;it_setr != it_setr_e && it_bt != it_bt_e; ++it_bt, ++it_setr)
        {
            EXPECT_EQ(*it_bt, *it_setr);
        }
    }
    EXPECT_EQ(1, 1);
}

TEST(BinaryTree, contains_remove_insert)
{
    BinaryTree<int> t;
    t.insert(1);
    t.insert(5);
    t.insert(3);
    t.insert(6);

    EXPECT_TRUE(t.contains(6));
    EXPECT_TRUE(t.contains(5));
    EXPECT_TRUE(t.contains(1));
    EXPECT_TRUE(t.contains(3));
    EXPECT_FALSE(t.contains(2));

    is_equal_collections(set{1,3,5,6}, t);
    t.remove(3); 
    is_equal_collections(set{1,5,6}, t);
    t.remove(1); 
    is_equal_collections(set{5,6}, t);
    t.remove(6); 
    is_equal_collections(set{5}, t);
    t.remove(5); 
    is_equal_collections(set<int>{}, t);
}

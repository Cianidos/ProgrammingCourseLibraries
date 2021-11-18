#include "pch.h"

#include <numeric>




#include <vector>
#include <list>

#include <boost/mpl/vector.hpp>

#include "list_sequence.h"
#include "sequence.h"
#include "linked_list.h"
#include "dynamic_array.h"

#include "testing.h"

using namespace std;
using namespace collections;
using namespace testing;

template<typename T>
using b = boost::mpl::vector<collections::linked_list<T>, std::vector<T>, std::list<T>, collections::dynamic_array<T>>;
using a = boost::mpl::vector<int, float, std::pair<int, int>>;


template <typename Lambda>
void type_combined_test(Lambda lambda)
{
    type_combined_test_template<Lambda, b, a>(lambda);
}

template <typename Lambda>
void type_double_combined_test(Lambda lambda)
{
	type_double_combined_test_template<Lambda, b, a>(lambda);
}



TEST(sequence, constructors)
{
    type_combined_test([]<typename T, typename C>(T ty, C co)
    {
        size_t n = 5;
        C l(n);
        generate(begin(l), end(l), random_generator<T>());
        T data[3] = {random_generator<T>(), random_generator<T>(), random_generator<T>()};

        sequence<C> a;
        EXPECT_EQ(a.size(), 0);

        sequence<C> b = C{ data[0], data[1], data[2] };
        EXPECT_EQ(b[0], data[0]);
        EXPECT_EQ(b[1], data[1]);
        EXPECT_EQ(b[2], data[2]);

        sequence<C> c(l);
        auto it = l.begin();
        EXPECT_EQ(c[0], *it++);
        EXPECT_EQ(c[1], *it++);
        EXPECT_EQ(c[2], *it++);
    });
}


TEST(sequence, cutting)
{
    type_combined_test([]<typename T, typename Container>(T ty, Container co) {
        size_t n = 5;
        Container ll(n);
        generate(begin(ll), end(ll), random_generator<T>());
        sequence<Container> seq(ll);
        {
            auto&& [l, r] = seq.cut(2);
            EXPECT_EQ(l[0], seq[0]);
            EXPECT_EQ(l[1], seq[1]);
            EXPECT_EQ(r[0], seq[2]);
            EXPECT_EQ(r[1], seq[3]);
            EXPECT_EQ(r[2], seq[4]);
            EXPECT_EQ(l.size(), 2);
            EXPECT_EQ(r.size(), 3);
            EXPECT_EQ(seq.size(), 5);
        }
        {
            auto seq_c = seq;
            auto&& [l, r] = seq.cut_mut(3);
            EXPECT_EQ(l[0], seq_c[0]);
            EXPECT_EQ(l[1], seq_c[1]);
            EXPECT_EQ(l[2], seq_c[2]);
            EXPECT_EQ(r[0], seq_c[3]);
            EXPECT_EQ(r[1], seq_c[4]);
            EXPECT_EQ(l.size(), 3);
            EXPECT_EQ(r.size(), 2);
            EXPECT_EQ(seq.size(), 0);
        }
    });
}

TEST(sequence, subsequenses)
{
    type_combined_test([]<typename T, typename Container>(T ty, Container co) {
        size_t n = 5;
        Container ll(n);
        generate(begin(ll), end(ll), random_generator<T>());
        sequence<Container> seq(ll);
        {
            auto l = seq.get_subsequence(0, 2);
            auto r = seq.get_subsequence(2);
            EXPECT_EQ(l[0], seq[0]);
            EXPECT_EQ(l[1], seq[1]);
            EXPECT_EQ(r[0], seq[2]);
            EXPECT_EQ(r[1], seq[3]);
            EXPECT_EQ(r[2], seq[4]);
            EXPECT_EQ(l.size(), 2);
            EXPECT_EQ(r.size(), 3);
            EXPECT_EQ(seq.size(), 5);
        }
        {
            auto seq_c = seq;
            auto l = seq.extract_subsequence(0, 3);
            auto r = std::move(seq);
            EXPECT_EQ(l[0], seq_c[0]);
            EXPECT_EQ(l[1], seq_c[1]);
            EXPECT_EQ(l[2], seq_c[2]);
            EXPECT_EQ(r[0], seq_c[3]);
            EXPECT_EQ(r[1], seq_c[4]);
            EXPECT_EQ(l.size(), 3);
            EXPECT_EQ(r.size(), 2);
            EXPECT_EQ(seq.size(), 0);
        }
    });
}
TEST(sequence, append_back_seq)
{
    type_double_combined_test(
   []<typename T, typename Container, typename Container2> 
                (T ty, Container co, Container2 co2) 
    {
        size_t n = 2;
        size_t m = 3;
        Container container1(n);
        generate(begin(container1), end(container1), random_generator<T>());

        Container2 container2(m);
        generate(begin(container2), end(container2), random_generator<T>());
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            seq.append_back_mut(container2);

            EXPECT_EQ(seq.size(), n + m);
            EXPECT_EQ(seq2.size(), m);

            for (size_t i = n; i < n + m; ++i)
            {
                EXPECT_EQ(seq[i], seq2[i - n]);
            }
        }
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            auto appended = seq.append_back(container2);

            EXPECT_EQ(appended.size(), n + m);
            EXPECT_EQ(seq.size(), n);
            EXPECT_EQ(seq2.size(), m);

            for (size_t i = n; i < n + m; ++i)
            {
                EXPECT_EQ(appended[i], seq2[i - n]);
            }
        }
    });
}

TEST(sequence, append_front_seq)
{
    type_double_combined_test(
   []<typename T, typename Container, typename Container2> 
                (T ty, Container co, Container2 co2) 
    {
        size_t n = 5;
        size_t m = 100;
        Container container1(n);
        generate(begin(container1), end(container1), random_generator<T>());

        Container2 container2(m);
        generate(begin(container2), end(container2), random_generator<T>());
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            seq.append_front_mut(container2);
        
            EXPECT_EQ(seq.size(), n + m);
            EXPECT_EQ(seq2.size(), m);
        
            for (size_t i = 0; i <  m; ++i)
            {
                EXPECT_EQ(seq[i], seq2[i]);
            }
        }
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            auto appended = seq.append_front(container2);

            EXPECT_EQ(appended.size(), n + m);
            EXPECT_EQ(seq.size(), n);
            EXPECT_EQ(seq2.size(), m);

            for (size_t i = 0; i <  m; ++i)
            {
                EXPECT_EQ(appended[i], seq2[i]);
            }
        }
    });
}

TEST(sequence, insert_seq)
{
    type_double_combined_test(
   []<typename T, typename Container, typename Container2> 
                (T ty, Container co, Container2 co2) 
    {
        size_t n = 5;
        size_t m = 100;
        Container container1(n);
        generate(begin(container1), end(container1), random_generator<T>());

        Container2 container2(m);
        generate(begin(container2), end(container2), random_generator<T>());
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            seq.insert_mut(0, container2);

            EXPECT_EQ(seq.size(), n + m);
            EXPECT_EQ(seq2.size(), m);

            for (size_t i = 0; i <  m; ++i)
            {
                EXPECT_EQ(seq[i], seq2[i]);
            }
        }
        {
            sequence<Container> seq(container1);
            sequence<Container> seq2(container2);
            auto appended = seq.append_front(container2);

            EXPECT_EQ(appended.size(), n + m);
            EXPECT_EQ(seq.size(), n);
            EXPECT_EQ(seq2.size(), m);

            for (size_t i = 0; i <  m; ++i)
            {
                EXPECT_EQ(appended[i], seq2[i]);
            }
        }
    });
}


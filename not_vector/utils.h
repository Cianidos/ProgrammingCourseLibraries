#pragma once

#include<istream>
#include<ostream>
#include<concepts>
#include<iterator>
#include<ranges>
#include <string>
#include<utility>


template<typename T>
concept has_input_operator = 
requires(std::istream input, T inst) {
    input >> inst;
};

template<typename T>
concept has_output_operator = 
requires(std::ostream output, T inst) {
    output << inst;
};

template<typename Container>
concept is_iterator_constructible = std::constructible_from
    <Container, typename Container::iterator, typename Container::iterator>;

template<typename T, typename Container>
concept is_size_constructible = 
    std::constructible_from<Container, size_t, T>
    && requires (size_t length)
{
    Container(length, T{}).size() == length;
};

template<typename T, template <typename, typename ...Args> typename Container>
concept size_constructible_dep = std::constructible_from
    <Container<T>, size_t, T> && requires (size_t length)
{
    Container<T>{length, T{}}.size() == length;
};

template<typename Container, typename OtherContainer>
concept is_same_intypes = 
    std::same_as<typename Container::value_type, typename OtherContainer::value_type>;

template<typename Container, typename T = typename Container::value_type>
concept is_container =
    std::ranges::range<Container>
    && is_iterator_constructible<Container>
    && is_size_constructible<T, Container>
    && std::regular<Container>;

    
template<typename Container, typename OtherContainer>
concept is_containers_with_same_intypes = 
    std::same_as<typename Container::value_type, typename OtherContainer::value_type>
    && is_container<Container> && is_container<OtherContainer>;

template<typename Ty>
concept is_sizable = requires (Ty t)
{
    {t.size()} -> std::same_as<size_t>;
};

template<typename T, typename Container>
concept is_indexable = requires (Container cont, size_t index)
{
    {cont[index]} -> std::same_as<T&>;
};

template<typename Ty>
concept sizable_dep = requires (Ty t)
{
    {t.size()} -> std::same_as<size_t>;
};

template<typename T, template <typename, typename ...Args> typename Container>
concept indexable_dep = requires (Container<T> cont, size_t index)
{
    {cont[index]} -> std::same_as<T&>;
};

template<std::input_iterator Iterator, std::integral Diff>
Iterator advance_s(Iterator iterator, Diff offset)
{
    std::advance(iterator, offset);
    return iterator;
}

template<typename IteratorLhs, typename IteratorRhs>
requires std::input_iterator<IteratorLhs> && std::input_iterator<IteratorRhs>
constexpr auto compare_collections
(
    IteratorLhs begin_it_lhs, IteratorLhs end_it_lhs,
    IteratorRhs begin_it_rhs, IteratorRhs end_it_rhs
)
{
    for (; begin_it_lhs != end_it_lhs && begin_it_rhs != end_it_rhs;
        ++begin_it_lhs, ++begin_it_rhs)
    {
        if (auto cmp = *(begin_it_lhs) <=> *(begin_it_rhs); cmp != 0)
        {
            return cmp;
        }
    }
    return std::distance(begin_it_lhs, end_it_lhs)
    <=> std::distance(begin_it_rhs, end_it_rhs);
}


template<typename RangeLhs, typename RangeRhs>
requires std::ranges::range<RangeLhs> && std::ranges::range<RangeRhs>
constexpr auto compare_collections(const RangeLhs& lhs, const RangeRhs& rhs)
{
    auto begin_it_lhs = std::begin(lhs),
        end_it_lhs = std::end(lhs);
    auto begin_it_rhs = std::begin(rhs),
        end_it_rhs = std::end(rhs);
    return compare_collections(begin_it_lhs, end_it_lhs, begin_it_rhs, end_it_rhs);
}

template<typename IteratorLhs, typename IteratorRhs>
//requires std::forward_iterator<IteratorLhs> && std::forward_iterator<IteratorRhs>
constexpr bool is_equal_collections
(
    IteratorLhs begin_it_lhs, IteratorLhs end_it_lhs,
    IteratorRhs begin_it_rhs, IteratorRhs end_it_rhs
)
{
    const size_t len_1 = std::distance(begin_it_lhs, end_it_lhs);
    const size_t len_2 = std::distance(begin_it_rhs, end_it_rhs);

    if (len_1 != len_2)
    { return false; }

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

template<typename RangeLhs, typename RangeRhs>
requires std::ranges::range<RangeLhs> && std::ranges::range<RangeRhs>
constexpr bool is_equal_collections(const RangeLhs& lhs, const RangeRhs& rhs)
{
    if constexpr (is_sizable<RangeLhs> && is_sizable<RangeRhs>)
    { if (lhs.size() != rhs.size()) return false; }

    auto begin_it_lhs = std::begin(lhs),
        end_it_lhs = std::end(lhs);
    auto begin_it_rhs = std::begin(rhs),
        end_it_rhs = std::end(rhs);
    return is_equal_collections(begin_it_lhs, end_it_lhs, begin_it_rhs, end_it_rhs);
}


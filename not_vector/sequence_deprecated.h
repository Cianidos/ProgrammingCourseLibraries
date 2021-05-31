#pragma once
#include <concepts>
#include "utils.h"

namespace deprecated
{
    template<typename V, typename R>
    concept is_linear_space = requires (V x, V y, V z, R l, R m)
    {
        {x + y} -> std::same_as<V>;
        {l * x} -> std::same_as<V>;

        x + y == y + x;
        (x + y) + z == x + (y + z);
        x + (x * R{0}) == x;
        x + (-x) == x * R{0};
        R{1} * x == x;
        l * (m * x) == (l * m) * x;
        l * (x + y) == (l * x) + (l * y);
        (l + m) * x == (l * x) + (m * x);
    };

    namespace sequence_companion
    {
        template <
            typename T,
            template <typename, typename ... Args> class Container,
            template <typename, typename ... Args> class OtherContainer
        >
            requires std::ranges::range<OtherContainer<T>>
            && std::ranges::range<Container<T>>
            && iterator_constructible<Container<T>>
            && size_constructible_dep<T, Container>
            && std::regular<Container<T>>
        Container<T> concat_containers(const Container<T>& lhs, const OtherContainer<T>& rhs)
        {
            Container<T> tmp(lhs);
            concat_containers_mut(tmp, rhs);
            return tmp;
        }


        template <
            typename T,
            template <typename, typename ... Args> class Container,
            template <typename, typename ... Args> class OtherContainer
        >
            requires std::ranges::range<OtherContainer<T>>
            && std::ranges::range<Container<T>>
            && iterator_constructible<Container<T>>
            && size_constructible<T, Container>
            && std::regular<Container<T>>
        void concat_containers_mut(Container<T>& lhs, OtherContainer<T>&& rhs)
        {
            Container<T> tmp(lhs.size() + rhs.size());
            auto end_it = std::copy_n(lhs.begin(), lhs.size(), tmp.begin());
            std::copy_n(rhs.begin(), rhs.size(), end_it);
            lhs = tmp;
        }

        template <
            typename T,
            template <typename, typename ... Args> class Container
        >
            requires std::ranges::range<Container<T>>
            && iterator_constructible<Container<T>>
            && size_constructible<T, Container>
            && std::regular<Container<T>>
        std::pair<Container<T>, Container<T>> cut_container
        (typename Container<T>::iterator index, const Container<T>& cont)
        {
            return cut_container_mut(index, cont);
        }

        template <
            typename T,
            template <typename, typename ... Args> class Container
        >
            requires std::ranges::range<Container<T>>
            && iterator_constructible<Container<T>>
            && size_constructible<T, Container>
            && std::regular<Container<T>>
        std::pair<Container<T>, Container<T>> cut_container_mut
        (typename Container<T>::iterator index, Container<T>& cont)
        {
            Container<T> left{cont.begin(), index};
            Container<T> right{index, cont.end()};
            return {left, right};
        }
    }


    template<
        typename T,
        template<typename, typename ...Args>
            typename Container
    >
    requires
    std::ranges::range<Container<T>>
    && iterator_constructible<Container<T>>
    && size_constructible<T, Container>
    && std::regular<Container<T>>
    class sequence
    {
    protected:
        Container<T> container_ = {};

    public:
        using iterator = typename Container<T>::iterator;
        using const_iterator = typename Container<T>::const_iterator;
        using reverse_iterator = typename Container<T>::reverse_iterator;
        using const_reverse_iterator = typename Container<T>::const_reverse_iterator;

        sequence() = default;
        sequence(const sequence&) = default;
        sequence(sequence&&) = default;

        sequence& operator=(const sequence&) = default;
        sequence& operator=(sequence&&) = default;

        ~sequence() = default;

        sequence(const Container<T>& container)
            : container_(container)
        { }

        sequence(Container<T>&& container)
            : container_(std::move(container))
        { }

        size_t size() const
        {
            if constexpr (sizable<Container<T>>)
                return container_.size();
            else
                return std::distance(std::begin(container_), std::end(container_));
        }

        const T& operator[](size_t index) const
        {
            if constexpr (indexable<T, Container>)
                return container_[index];
            else
            {
                auto it = std::begin(container_);
                std::ranges::advance(it, index);
                return *it;
            }
        }


        std::pair<sequence, sequence> cut(const_iterator index) const
        {
            auto [left, right]
                = sequence_companion::cut_container(index, container_);
            return { sequence{left}, sequence{right} };
        }

        std::pair<sequence, sequence> cut(size_t index) const
        {
            auto it = begin();
            std::advance(it, index);
            return cut(it);
        }

        std::pair<sequence, sequence> cut_mut(iterator index)
        {
            auto [left, right]
                = sequence_companion::cut_container_mut(index, std::move(container_));
            return { sequence{left}, sequence{right} };
        }

        std::pair<sequence, sequence> cut_mut(size_t index)
        {
            auto it = begin();
            std::advance(it, index);
            return cut_mut(it);
        }

        sequence extract_subsequence(iterator left_it, iterator right_it)
        {
            size_t dist = std::distance(left_it, right_it);
            auto [left, tail]
                = sequence_companion::cut_container_mut(left_it, std::move(container_));
            auto new_right_it = std::advance(tail.begin(), dist);
            auto [middle, right]
                = sequence_companion::cut_container_mut(new_right_it, std::move(tail));
            sequence_companion::concat_containers_mut(left, right);
            container_ = left;
            return middle;
        }

        sequence extract_subsequence(size_t left_index = 0, size_t right_index = static_cast<size_t>(-1)) const
        {
            auto it = begin();
            std::advance(it, (right_index == static_cast<size_t>(-1) ? this->size() : right_index));
            return extract_subsequence(std::advance(begin(), left_index), it);
        }

        sequence get_subsequence(iterator left_it, iterator right_it) const
        {
            sequence tmp{ Container<T>{left_it, right_it} };
            return tmp;
        }

        sequence get_subsequence(size_t left_index = 0, size_t right_index = static_cast<size_t>(-1)) const
        {
            auto it = begin();
            std::advance(it, (right_index == static_cast<size_t>(-1) ? this->size() : right_index));
            return get_subsequence(std::advance(begin(), left_index), it);
        }

        Container<T>&& release_container()
        {
            return std::move(container_);
        }

        sequence append_back(const sequence& other) const
        {
            return append_back<Container>(other);
        }

        sequence append_front(const sequence& other) const
        {
            return append_front<Container>(other);
        }

        sequence insert(size_t index, const sequence& other) const
        {
            return insert<Container>(index, other);
        }

        sequence insert(iterator index, const sequence& other) const
        {
            return insert<Container>(index, other);
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            sequence append_back(const sequence<T, OtherContainer>& other) const
        {
            Container<T> tmp = sequence_companion::concat_containers(container_, other.container_);
            return { tmp };
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            sequence append_front(const sequence<T, OtherContainer>& other) const
        {
            Container<T> tmp = sequence_companion::concat_containers(other.container_, container_);
            return { tmp };
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            sequence insert(size_t index, const sequence<T, OtherContainer>& other) const
        {
            auto it = begin();
            std::advance(it, index);
            return insert(it);
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            sequence insert(iterator index, const sequence<T, OtherContainer>& other) const
        {
            auto [left, right] = sequence_companion::cut_container(index, container_);
            Container<T> middle = Container<T>(other);

            sequence_companion::concat_containers_mut(left, std::move(middle));
            sequence_companion::concat_containers_mut(left, std::move(right));

            return { left };
        }

        sequence append_back(T&& value) const
        {
            Container<T> tmp(1, std::forward<T>(value));
            return append_back<Container>(tmp);
        }

        sequence append_front(T&& value) const
        {
            Container<T> tmp(1, std::forward<T>(value));
            return append_front<Container>(tmp);
        }

        sequence insert(size_t index, T&& value) const
        {
            Container<T> tmp(1, std::forward<T>(value));
            return insert<Container>(index, tmp);
        }

        sequence insert(iterator index, T&& value) const
        {
            Container<T> tmp(1, std::forward<T>(value));
            return insert<Container>(index, tmp);
        }


        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            void append_back_mut(sequence<T, OtherContainer> other)
        {
            sequence_companion::concat_containers_mut<T, Container, OtherContainer>(container_, std::move(other.container_));
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            void append_front_mut(sequence<T, OtherContainer> other)
        {
            sequence_companion::concat_containers_mut<T, OtherContainer, Container>(other.container_, std::move(container_));
            container_ = std::move(other);
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            void insert_mut(size_t index, sequence<T, OtherContainer> other)
        {
            auto it = begin();
            std::advance(it, index);
            return insert_mut(it, index);
        }

        template<template<typename, typename ...Args> typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer<T>>
            void insert_mut(iterator index, sequence<T, OtherContainer> other)
        {
            auto [left, right] = sequence_companion::cut_container_mut(index, std::move(container_));
            Container<T> middle = Container<T>(other.container_);

            sequence_companion::concat_containers_mut(left, std::move(middle));
            sequence_companion::concat_containers_mut(left, std::move(right));

            return { left };
        }

        void append_back_mut(sequence other)
        {
            return append_back_mut<Container>(other);
        }

        void append_front_mut(sequence other)
        {
            return append_front_mut<Container>(other);
        }

        void insert_mut(size_t index, sequence other)
        {
            return insert_mut<Container>(index, other);
        }

        void insert_mut(iterator index, sequence other)
        {
            return insert_mut<Container>(index, other);
        }


        void append_back_mut(T&& value)
        {
            Container<T> tmp(1, std::forward<T>(value));
            append_back_mut<Container>(tmp);
        }

        void append_front_mut(T&& value)
        {
            Container<T> tmp(1, std::forward<T>(value));
            append_front_mut<Container>(tmp);
        }

        void insert_mut(size_t index, T&& value)
        {
            Container<T> tmp(1, std::forward<T>(value));
            insert_mut<Container>(index, tmp);
        }

        void insert_mut(iterator index, T&& value)
        {
            Container<T> tmp(1, std::forward<T>(value));
            insert_mut<Container>(index, tmp);
        }


        iterator begin()
        {
            return std::begin(container_);
        }

        iterator end()
        {
            return std::end(container_);
        }

        const_iterator begin() const
        {
            return std::begin(container_);
        }

        const_iterator end() const
        {
            return std::end(container_);
        }

        const_iterator cbegin() const
        {
            return std::cbegin(container_);
        }

        const_iterator cend() const
        {
            return std::cend(container_);
        }

        reverse_iterator rbegin()
        {
            return std::rbegin(container_);
        }

        reverse_iterator rend()
        {
            return std::rend(container_);
        }

        const_reverse_iterator rbegin() const
        {
            return std::rbegin(container_);
        }

        const_reverse_iterator rend() const
        {
            return std::rend(container_);
        }

        const_reverse_iterator crbegin() const
        {
            return std::crbegin(container_);
        }

        const_reverse_iterator crend() const
        {
            return std::crend(container_);
        }

        template<template<typename, typename ...Args> typename OtherContainer>
        requires std::ranges::range<OtherContainer<T>>
            bool operator==(const sequence<T, OtherContainer>& other) const
        {
            return is_equal_collections(this->container_, other.container_);
        }

        template<template<typename, typename ...Args> typename OtherContainer>
        requires std::ranges::range<OtherContainer<T>>
            bool operator<=>(const sequence<T, OtherContainer>& other) const
        {
            return compare_collections(this->container_, other.container_);
        }
    };

}

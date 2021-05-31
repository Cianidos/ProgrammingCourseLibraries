#pragma once
#include <concepts>
#include "utils.h"


namespace collections
{
    namespace sequence_companion
    {
        template <typename Container, typename OtherContainer>
        requires is_containers_with_same_intypes<Container, OtherContainer>
            constexpr void concat_containers_mut(Container& lhs, OtherContainer rhs)
        {
            Container tmp(lhs.size() + rhs.size());
            auto end_it = std::move(lhs.begin(), lhs.end(), tmp.begin());
            std::move((rhs.begin()), rhs.end(), end_it);
            lhs = tmp;
        }

        template <typename Container, typename OtherContainer>
        requires is_containers_with_same_intypes<Container, OtherContainer>
            constexpr Container concat_containers(const Container& lhs, const OtherContainer& rhs)
        {
            Container tmp(lhs);
            concat_containers_mut(tmp, rhs);
            return tmp;
        }


        template <typename Container>
        requires is_container<Container>
            constexpr std::pair<Container, Container> cut_container_mut
            (typename Container::iterator index, Container& cont)
        {
            Container left{ std::make_move_iterator(cont.begin()), std::make_move_iterator(index) };
            Container right{ std::make_move_iterator(index), std::make_move_iterator(cont.end()) };
            return { left, right };
        }

        template <typename Container>
        requires is_container<Container>
            constexpr std::pair<Container, Container> cut_container
            (typename Container::const_iterator index, const Container& cont)
        {
            Container tmp(cont);
            auto it = tmp.begin();
            std::advance(it, std::distance(cont.cbegin(), index));
            auto res = cut_container_mut<Container>(it, tmp);
            return res;
        }

    }



    template<typename Container>
    requires is_container<Container>
    class sequence
    {
    protected:
        Container container_ = {};

    public:
        using T = typename Container::value_type;
        using value_type = typename Container::value_type;
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;
        using reverse_iterator = typename Container::reverse_iterator;
        using const_reverse_iterator = typename Container::const_reverse_iterator;

        constexpr sequence() = default;
        constexpr sequence(const sequence&) = default;
        constexpr sequence(sequence&&) noexcept = default;

        constexpr sequence& operator=(const sequence&) = default;
        constexpr sequence& operator=(sequence&&) = default;

        constexpr ~sequence() = default;

        constexpr sequence(const Container& container)
            : container_(container) { }

        constexpr sequence(Container&& container)
            : container_(std::move(container)) { }

        template<is_container OtherContainer>
        constexpr sequence(const OtherContainer& container)
            : container_(std::begin(container), std::end(container)) { }

        template<is_container OtherContainer>
        constexpr sequence(OtherContainer&& container)
            : container_(std::move_iterator(std::begin(container)),
                std::move_iterator(std::end(container))) { }

        consteval size_t csize() const
        {
            if constexpr (is_sizable<Container>)
                return container_.size();
            else
                return std::distance(std::begin(container_), std::end(container_));
        }

        constexpr size_t size() const
        {
            if constexpr (is_sizable<Container>)
                return container_.size();
            else
                return std::distance(std::begin(container_), std::end(container_));
        }

        constexpr const T& operator[](size_t index) const
        {
            if constexpr (is_indexable<T, Container>)
                return container_[index];
            else
            {
                auto it = std::begin(container_);
                std::ranges::advance(it, index);
                return *it;
            }
        }
        constexpr T& operator[](size_t index) 
        {
            if constexpr (is_indexable<T, Container>)
                return container_[index];
            else
            {
                auto it = std::begin(container_);
                std::ranges::advance(it, index);
                return *it;
            }
        }

        constexpr std::pair<sequence, sequence> cut(const_iterator index) const
        {
            auto [left, right]
                = sequence_companion::cut_container<Container>(index, container_);
            return { sequence{left}, sequence{right} };
        }

        constexpr std::pair<sequence, sequence> cut(size_t index) const
        { return cut(iterator_by_index(index)); }

        constexpr std::pair<sequence, sequence> cut_mut(iterator index)
        {
            auto [left, right]
                = sequence_companion::cut_container_mut(index, (container_));
            container_ = {};
            return { sequence{(left)}, sequence{(right)} };
        }

        constexpr std::pair<sequence, sequence> cut_mut(size_t index)
        { return cut_mut(iterator_by_index(index)); }

        constexpr sequence extract_subsequence(iterator left_it, iterator right_it)
        {
            size_t dist = std::distance(left_it, right_it);
            auto [left, tail]
                = sequence_companion::cut_container_mut(left_it, (container_));
            auto new_right_it = advance_s(tail.begin(), dist);
            auto [middle, right]
                = sequence_companion::cut_container_mut(new_right_it, (tail));
            sequence_companion::concat_containers_mut(left, right);
            container_ = left;
            return middle;
        }

        constexpr sequence extract_subsequence(size_t left_index = 0, size_t right_index = static_cast<size_t>(-1))
        {
            right_index = (right_index == static_cast<size_t>(-1) ? this->size() : right_index);
            auto [it_left, it_right] = range_by_indexes(left_index, right_index);
            return extract_subsequence(it_left, it_right);
        }

        constexpr sequence get_subsequence(const_iterator left_it, const_iterator right_it) const
        { return  { Container{left_it, right_it} }; }

        constexpr sequence get_subsequence(size_t left_index = 0, size_t right_index = static_cast<size_t>(-1)) const
        {
            right_index = (right_index == static_cast<size_t>(-1) ? this->size() : right_index);
            auto [it_left, it_right] = range_by_indexes(left_index, right_index);
            return get_subsequence(it_left, it_right);
        }

        constexpr Container&& release_container()
        { return std::move(container_); }

        constexpr sequence append_back(const sequence& other) const
        { return append_back<Container>(other); }

        constexpr sequence append_front(const sequence& other) const
        { return append_front<Container>(other); }

        constexpr sequence insert(size_t index, const sequence& other) const
        { return insert<Container>(index, other); }

        constexpr sequence insert(iterator index, const sequence& other) const
        { return insert<Container>(index, other); }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            sequence append_back(const sequence<OtherContainer>& other) const
        { return { std::move(sequence_companion::concat_containers(container_, other.container_)) }; }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            sequence append_front(const sequence<OtherContainer>& other) const
        { return { std::move(sequence_companion::concat_containers(other.container_, container_)) }; }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            sequence insert(size_t index, const sequence<OtherContainer>& other) const
        { return insert(iterator_by_index(index)); }


        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            sequence insert(iterator index, const sequence<OtherContainer>& other) const
        {
            auto [left, right] = sequence_companion::cut_container(index, container_);
            Container middle = Container(other);

            sequence_companion::concat_containers_mut(left, std::move(middle));
            sequence_companion::concat_containers_mut(left, std::move(right));

            return { std::move(left) };
        }
        constexpr sequence append_back(T&& value) const
        { return append_back<Container>(Container( 1, std::forward<T>(value) )); }

        constexpr sequence append_front(T&& value) const
        { return append_front<Container>(Container( 1, std::forward<T>(value) )); }

        constexpr sequence insert(size_t index, T&& value) const
        { return insert<Container>(index, Container( 1, std::forward<T>(value) )); }
    
        constexpr sequence insert(iterator index, T&& value) const
        { return insert<Container>(index, Container( 1, std::forward<T>(value) )); }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            void append_back_mut(sequence<OtherContainer> other)
        {
            sequence_companion::concat_containers_mut<Container, OtherContainer>
                (container_, std::move(other.container_));
        }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            void append_front_mut(sequence<OtherContainer> other)
        {
            sequence_companion::concat_containers_mut<OtherContainer, Container>(other.container_, std::move(container_));
            container_ = std::move(other.container_);
        }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            void insert_mut(size_t index, sequence<OtherContainer> other)
        { return insert_mut(iterator_by_index(index), std::move(other)); }

        template<typename OtherContainer = Container>
        requires std::ranges::range<OtherContainer>
            void insert_mut(iterator index, sequence<OtherContainer> other)
        {
            auto [left, right] = sequence_companion::cut_container_mut(index, (container_));

            sequence_companion::concat_containers_mut<Container, OtherContainer>(left, std::move(other.container_));
            sequence_companion::concat_containers_mut<Container, OtherContainer>(left, std::move(right));

            container_ = std::move(left);
        }

        constexpr void append_back_mut(sequence other)
        { return append_back_mut<Container>(std::move(other)); }

        constexpr void append_front_mut(sequence other)
        { return append_front_mut<Container>(std::move(other)); }

        constexpr void insert_mut(size_t index, sequence other)
        { return insert_mut<Container>(index, std::move(other)); }

        constexpr void insert_mut(iterator index, sequence other)
        { return insert_mut<Container>(index, std::move(other)); }
    
        constexpr void append_back_mut(T&& value)
        { append_back_mut<Container>({ 1, std::forward<T>(value) }); }

        constexpr void append_front_mut(T&& value)
        { append_front_mut<Container>({ 1, std::forward<T>(value) }); }

        constexpr void insert_mut(size_t index, T&& value)
        { insert_mut<Container>(index, { 1, std::forward<T>(value) }); }

        constexpr void insert_mut(iterator index, T&& value)
        { insert_mut<Container>(index, { 1, std::forward<T>(value) }); }

        constexpr iterator begin()
        { return std::begin(container_); }
        constexpr iterator end()
        { return std::end(container_); }
        constexpr const_iterator begin() const
        { return std::begin(container_); }
        constexpr const_iterator end() const
        { return std::end(container_); }
        constexpr const_iterator cbegin() const
        { return std::cbegin(container_); }
        constexpr const_iterator cend() const
        { return std::cend(container_); }
        constexpr reverse_iterator rbegin()
        { return std::rbegin(container_); }
        constexpr reverse_iterator rend()
        { return std::rend(container_); }
        constexpr const_reverse_iterator rbegin() const
        { return std::rbegin(container_); }
        constexpr const_reverse_iterator rend() const
        { return std::rend(container_); }
        constexpr const_reverse_iterator crbegin() const
        { return std::crbegin(container_); }
        constexpr const_reverse_iterator crend() const
        { return std::crend(container_); }

        template<typename OtherContainer>
        requires std::ranges::range<OtherContainer>
            bool operator==(const sequence<OtherContainer>& other) const
        { return is_equal_collections(this->container_, other.container_); }

        template<typename OtherContainer>
        requires std::ranges::range<OtherContainer>
            bool operator<=>(const sequence<OtherContainer>& other) const
        { return compare_collections(this->container_, other.container_); }

    protected:

        constexpr iterator iterator_by_index(size_t index)
        {
            if (index > size())
                throw std::out_of_range(
                    "Sequence index out of range, index = "
                    + std::to_string(index)
                    + " when size() = "
                    + std::to_string(size()) + "\n");

            return advance_s(begin(), index);
        }

        constexpr const_iterator iterator_by_index(size_t index) const
        {
            if (index > size())
                throw std::out_of_range(
                    "Sequence index out of range, index = "
                    + std::to_string(index)
                    + " when size() = "
                    + std::to_string(size()) + "\n");

            return advance_s(begin(), index);
        }

        constexpr std::pair<iterator, iterator>
        range_by_indexes(size_t left, size_t right)
        {
            if (right > size())
                throw std::out_of_range(
                    "Sequence index out of range, right = "
                    + std::to_string(right)
                    + " when size() = "
                    + std::to_string(size()) + "\n");

            if (left > right)
                throw std::invalid_argument(
                    "Sequence out of range left > right, right = "
                    + std::to_string(right)
                    + " when left = "
                    + std::to_string(left) + "\n");

            auto l = advance_s(begin(), left);
            return { l, advance_s(l, right - left) };
        }

        constexpr std::pair<const_iterator, const_iterator>
        range_by_indexes(size_t left, size_t right) const
        {
            if (right > size())
                throw std::out_of_range(
                    "Sequence index out of range, right = " + std::to_string(right)
                    + " when size() = " + std::to_string(size()) + "\n");

            if (left > right)
                throw std::invalid_argument(
                    "Sequence out of range left > right, right = " + std::to_string(right)
                    + " when left = " + std::to_string(left) + "\n");

            auto l = advance_s(begin(), left);
            return { l, advance_s(l, right - left) };
        }
    };
}

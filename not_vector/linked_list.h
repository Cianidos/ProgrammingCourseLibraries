#pragma once

#include <list>
#include <memory>
#include <functional>
#include <stdexcept>
#include <string>
#include <array>
#include <iterator>
#include <ranges>
#include <concepts>

#include "utils.h"

namespace collections
{
    template<typename ValueType>
    class linked_list;

    template<typename ValueType>
    class linked_list_node
    {
    private:

        friend linked_list<ValueType>;

        constexpr void linkage() noexcept
        {
            if (next) next->previous = this;
            if (previous) previous->next = this;
        }
    
        constexpr void de_linkage() noexcept
        {
            if (next != nullptr && previous != nullptr)
            {
                next->previous = previous;
                previous->next = next;
            }
            else if (next != nullptr)
            {
                next->previous = nullptr;
            }
            else if (previous != nullptr)
            {
                previous->next = nullptr;
            } else {/* do nothing */}
        }

    public:
        linked_list_node* next;
        linked_list_node* previous;
        ValueType value;


        constexpr linked_list_node() = delete;
        linked_list_node(const linked_list_node& other) = default;
        linked_list_node(linked_list_node&& other) noexcept = default;

        linked_list_node& operator=(const linked_list_node& other) = default;
        linked_list_node& operator=(linked_list_node&& other) noexcept = default;

        constexpr explicit linked_list_node(
            const ValueType& value, 
            linked_list_node* next = nullptr, 
            linked_list_node* previous = nullptr
        ) noexcept
            : next(next), previous(previous), value(value)
        { linkage(); }

        constexpr explicit linked_list_node(
            ValueType&& value, 
            linked_list_node* next = nullptr, 
            linked_list_node* previous = nullptr
        ) noexcept
            : next(next), previous(previous), value(std::move(value))
        { linkage(); }


        constexpr auto operator==(const linked_list_node& other) const noexcept
        { return *this == other; }

        constexpr auto operator<=>(const linked_list_node& other) const noexcept
        { return *this <=> other; }

        constexpr ~linked_list_node() noexcept
        { de_linkage(); }
    };


    template<typename T, bool Const>
    class node_iterator_impl
    {
        using pointer_impl = std::conditional_t<!Const, T*, const T*>; 
        using reference_impl = std::conditional_t<!Const, T&, const T&>; 

        constexpr linked_list_node<T>* next_node()
        { return current_node_->next; }

        constexpr linked_list_node<T>* previous_node()
        { return current_node_->previous; }

    public:
        using value_type        = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = ptrdiff_t;
        using pointer           = pointer_impl;
        using reference         = reference_impl;

        constexpr node_iterator_impl() noexcept = default;
        constexpr node_iterator_impl(const node_iterator_impl&) noexcept = default;
        constexpr node_iterator_impl(node_iterator_impl&&) noexcept = default;

        constexpr node_iterator_impl& operator=(const node_iterator_impl&) noexcept = default;
        constexpr node_iterator_impl& operator=(node_iterator_impl&&) noexcept = default;

        constexpr ~node_iterator_impl() noexcept = default;

        constexpr explicit node_iterator_impl(
            linked_list_node<T>* node, 
            const bool on_edge = false
        ) noexcept : current_node_(node), list_edge_(on_edge) {}

        constexpr node_iterator_impl& operator++()
        {
            if (list_edge_ == true)
            {
                throw std::runtime_error
                ("list iterator out of range: access to ++(c)(r)end");
            }
            if (next_node() != nullptr)
            { current_node_ = next_node(); }
            else
            { list_edge_ = true; }

            return *this;
        }

        constexpr node_iterator_impl operator++(int) 
        {
            node_iterator_impl tmp(current_node_, list_edge_);
            ++(*this);
            return tmp;
        }

        constexpr node_iterator_impl& operator--()
        {
            if (previous_node() == nullptr)
            {
                throw std::runtime_error
                ("list iterator out of range: access to --(c)(r)begin");
            }
            if (list_edge_ == false)
            { current_node_ = previous_node(); }
            else
            { list_edge_ = false; }

            return *this;
        }

        constexpr node_iterator_impl operator--(int)
        {
            node_iterator_impl tmp(current_node_, list_edge_);
            --(*this);
            return tmp;
        }

        constexpr bool operator==(const node_iterator_impl& other) const noexcept
        {
            return
            other.current_node_ == current_node_
            && other.list_edge_ == list_edge_
            || 
            other.current_node_ == nullptr
            && current_node_ == nullptr;
        }

        constexpr auto operator<=>(const node_iterator_impl& other) const noexcept = delete;

        constexpr reference operator*() const noexcept
        { return current_node_->value; }

        constexpr pointer operator->() const noexcept
        { return &current_node_->value; }

    private:
        friend linked_list<T>;
        linked_list_node<T>* current_node_ = nullptr;
        bool list_edge_ = false;
    };


    template<typename T>
    class linked_list
    {
    public:
        using value_type = T;
        using iterator = node_iterator_impl<value_type, false>;
        using const_iterator = node_iterator_impl<value_type, true>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr linked_list() noexcept = default;

        explicit constexpr
        linked_list(size_t count, const value_type& default_value = {})
        {
            for(;count > 0; --count)
            { emplace_back(default_value); }
        }
    
        template<typename IteratorType>
        requires std::input_iterator<IteratorType>
        constexpr linked_list(IteratorType begin_it, IteratorType end_it)
        {
            while (begin_it != end_it)
            {
                emplace_back(*begin_it);
                ++begin_it;
            }
        }

        constexpr linked_list(std::initializer_list<value_type> list)
        {
            for (const auto& i : list)
            { emplace_back(i); }
        }

        constexpr linked_list(const linked_list& other)
        { *this = other; }
    
        constexpr linked_list(linked_list&& other) noexcept
        { *this = std::move(other); }

        constexpr linked_list& operator=(const linked_list& other)
        {
            if (this != &other)
            {
                linked_list tmp(std::move(*this));
                linked_list_node<value_type>* node = other.head_;
                while (node != nullptr)
                {
                    emplace_back(node->value);
                    node = node->next;
                }
            }
            return *this;
        }

        constexpr linked_list& operator=(linked_list&& other) noexcept
        {
            if(this != &other)
            {
                linked_list tmp;
                std::swap(head_ , tmp.head_);
                std::swap(tail_ , tmp.tail_);
                std::swap(size_ , tmp.size_);
                std::swap(head_ , other.head_);
                std::swap(tail_ , other.tail_);
                std::swap(size_ , other.size_);

            }
            return *this;
        }

        [[nodiscard]] constexpr size_t size() const noexcept
        { return size_; }

        constexpr value_type& operator[](const size_t index)
        { return node_by_index(index)->value; }
    
        constexpr const value_type& operator[](const size_t index) const
        { return node_by_index(index)->value; }

        constexpr value_type& at(const size_t index)
        { return node_by_index(index)->value; }
    
        constexpr const value_type& at(const size_t index) const
        { return node_by_index(index)->value; }

        constexpr iterator push_back(const value_type& value)
        { return emplace_back(value); }

        constexpr iterator push_back(value_type&& value)
        { return emplace_back(value); }

        constexpr iterator push_front(const value_type& value)
        { return emplace_front(value); }

        constexpr iterator push_front(value_type&& value)
        { return emplace_front(value); }

        constexpr iterator insert(const size_t index, const value_type& value)
        { return emplace(index, value); }

        constexpr iterator insert(const size_t index, value_type&& value)
        { return emplace(index, value); }

        constexpr value_type pop_back()
        {
            if (size_ == 0)
            { throw std::runtime_error("list size = 0"); }

            value_type tmp = std::move(tail_->value);
            auto* tmp_node = tail_->previous;
            delete tail_;
            tail_ = tmp_node;
            size_ -=1;
            return tmp;
        }

        constexpr value_type pop_front()
        {
            if (size_ == 0)
            { throw std::runtime_error("list size = 0"); }

            value_type tmp = std::move(head_->value);
            auto* tmp_node = head_->next;
            delete head_;
            head_ = tmp_node;
            size_ -= 1;
            return tmp;
        }

        constexpr value_type&& extract(const size_t index)
        {
            linked_list_node<value_type>* node = node_by_index(index);
            value_type tmp = std::move(node->value);
            delete node;
            return std::move(tmp);
        }

        template<class ...ArgsTy>
        constexpr iterator emplace_back(ArgsTy&& ...args)
        {
            tail_ = new linked_list_node<value_type>
{ value_type{std::forward<ArgsTy>(args) ...}, nullptr, tail_ };

            if (size_ == 0) { head_ = tail_; }
            size_ += 1;
            return iterator(tail_);
        }

        template<class ...ArgsTy>
        constexpr iterator emplace_front(ArgsTy&& ...args)
        {
            head_ = new linked_list_node<value_type>
{ value_type{std::forward<ArgsTy>(args) ...}, head_ };

            if (size_ == 0) { tail_ = head_; }
            size_ += 1;
            return iterator(head_);
        }

        template<class ...ArgsTy>
        constexpr iterator emplace(const size_t index, ArgsTy&& ...args)
        {
            linked_list_node<value_type>* in_index = node_by_index(index);
            auto tmp = new linked_list_node<value_type>
{ value_type{std::forward<ArgsTy>(args) ...}, in_index, in_index->previous };
            size_ += 1;
            return iterator(tmp);
        }


        constexpr void concat(linked_list other)
        {
            if (tail_ != nullptr)
                linked_list_node<value_type> tmp({}, other.head_, this->tail_);
            else
                head_ = other.head_;

            tail_ = other.tail_;
            size_ += other.size_;
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }

        constexpr iterator erase(const_iterator index)
        {
            if (index == cend()) throw std::invalid_argument("index = cend()");
            iterator res(index.current_node_->next, index.current_node_->next == tail_->previous);
            delete index.current_node_;
            --size_;
            return res;
        }
        constexpr iterator erase(const_iterator left_it, const_iterator right_it)
        {
            size_ -= std::distance(left_it, right_it);
            while(left_it != right_it)
            {
                ++left_it;
                delete left_it.current_node_->previous;
            }
            return iterator(left_it.current_node_, left_it.list_edge_);
        }

        constexpr iterator erase(size_t index)
        {
            auto it = cbegin();
            std::advance(it, index);
            return erase(it);
        }

        constexpr iterator erase(size_t left_index, size_t right_index)
        {
            auto it_l = cbegin();
            std::advance(it_l, left_index);
            auto it_r = it_l;
            std::advance(it_r, right_index - left_index);
            return erase(it_l, it_r);
        }

    private: constexpr std::pair<linked_list, linked_list>
        cut(linked_list_node<value_type>* edge, const size_t index)
    {
        linked_list right;
        right.head_ = edge;
        right.tail_ = tail_;
        right.size_ = size_ - index;

        size_ = index;

        tail_ = edge->previous;

        if (tail_ != nullptr)
            tail_->next = nullptr;
        else
            head_ = nullptr;

        right.head_->previous = nullptr;

        linked_list left(std::move(*this));

        return {std::move(left), (right)};
    }

    public:
        constexpr std::pair<linked_list, linked_list>
        cut(iterator edge)
        {
            return cut(edge.current_node_, std::distance(begin(), edge));
        }

        constexpr std::pair<linked_list, linked_list>
        cut(size_t index)
        {
            return cut(node_by_index(index), index);
        }


        constexpr iterator begin() noexcept
        { return iterator{head_}; }
        constexpr iterator end() noexcept
        { return iterator{tail_, true}; }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        { return const_iterator{head_}; }
        [[nodiscard]] constexpr const_iterator end() const noexcept
        { return const_iterator{tail_, true}; }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        { return const_iterator{head_}; }
        [[nodiscard]] constexpr const_iterator cend() const noexcept
        { return const_iterator{tail_, true}; }


        constexpr reverse_iterator rbegin() noexcept
        { return std::make_reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept
        { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
        { return std::make_reverse_iterator(end()); }
        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
        { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr const_reverse_iterator rcbegin() const noexcept
        { return std::make_reverse_iterator(cend()); }
        [[nodiscard]] constexpr const_reverse_iterator rcend() const noexcept
        { return std::make_reverse_iterator(cbegin()); }

        constexpr void swap(linked_list& other) noexcept
        {
            linked_list tmp(std::move(other));
            other = std::move(*this);
            *this = std::move(tmp);
        }

        constexpr void clear() noexcept
        {
            while(size_ != 0)
            {
                try{ pop_back(); }
                catch (...) { return; }
            }
        }

        template<typename Container>
        requires std::ranges::range<Container>
        constexpr auto operator<=>(const Container& container) const noexcept
        { return compare_collections(*this, container); }

        template<typename Container>
        requires std::ranges::range<Container>
        constexpr bool operator==(const Container& container) const noexcept
        { return is_equal_collections(*this, container); }

        constexpr ~linked_list() noexcept
        { clear(); }

    private:
        [[nodiscard]] constexpr linked_list_node<value_type>* node_by_index(const size_t index) const
        {
            if (index >= size_)
            {
                throw std::out_of_range(
                    "in linked_list index out of range (index) " 
                    + std::to_string(index) + " >= (size) " + std::to_string(size_));
            }
            linked_list_node<value_type>* node;
            if (size_ / 2 > index)
            {
                node = head_;
                for (size_t i = 0; i < index; ++i)
                { node = node->next; }
            }
            else
            {
                node = tail_;
                for (size_t i = size_ - index - 1; i > 0; --i)
                { node = node->previous; }
            }
            return node;
        }

        linked_list_node<value_type>* head_ = nullptr;
        linked_list_node<value_type>* tail_ = nullptr;
        size_t size_= 0;
    };

    template<typename ValueType, template<typename , typename ...Args> typename  IteratorType>
    linked_list(IteratorType<ValueType>, IteratorType<ValueType>)
    -> linked_list<ValueType>;

    template<typename  IteratorType>
    linked_list(IteratorType, IteratorType)
    -> linked_list<typename std::iterator_traits<IteratorType>::value_type>;
}

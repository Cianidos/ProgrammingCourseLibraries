#pragma once
#include <iterator>

#include "utils.h"
namespace collections
{
    template<typename T, bool Const>
    class dynamic_array_iterator
    {
    public:
        using value_type = T;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = ptrdiff_t;
        using pointer = std::conditional_t<Const, const T*, T*>;
        using reference = std::conditional_t<Const, const T&, T&>;

        constexpr dynamic_array_iterator() = default;
        constexpr dynamic_array_iterator(const dynamic_array_iterator&) = default;
        constexpr dynamic_array_iterator(dynamic_array_iterator&&) = default;

        constexpr dynamic_array_iterator& operator=(const dynamic_array_iterator&) = default;
        constexpr dynamic_array_iterator& operator=(dynamic_array_iterator&&) = default;

        constexpr ~dynamic_array_iterator() = default;
    

        constexpr explicit dynamic_array_iterator(pointer element) noexcept
            : current_element_(element)
        { }

        constexpr dynamic_array_iterator& operator++() noexcept
        {
            ++current_element_;
            return *this;
        }

        constexpr dynamic_array_iterator operator++(int) noexcept
        {
            auto tmp(*this);
            ++(*this);
            return tmp;
        }

        constexpr dynamic_array_iterator& operator--() noexcept
        {
            --current_element_;
            return *this;
        }

        constexpr dynamic_array_iterator operator--(int) noexcept
        {
            auto tmp(*this);
            --(*this);
            return tmp;
        }

        constexpr dynamic_array_iterator& operator+=(const difference_type offset) noexcept
        {
            current_element_ += offset;
            return *this;
        }

        constexpr dynamic_array_iterator& operator-=(const difference_type offset) noexcept
        {
            current_element_ -= offset;
            return *this;
        }

        constexpr dynamic_array_iterator operator+(const difference_type offset) const noexcept
        {
            dynamic_array_iterator tmp(*this);
            return (tmp += offset);
        }

        constexpr dynamic_array_iterator operator-(const difference_type offset) const noexcept
        {
            dynamic_array_iterator tmp(*this);
            return (tmp -= offset);
        }

        constexpr difference_type operator-(const dynamic_array_iterator& other) const noexcept
        {
            return (current_element_ - other.current_element_);
        }

        constexpr auto operator<=>(const dynamic_array_iterator& other) const noexcept = default;
        constexpr bool operator==(const dynamic_array_iterator& other) const noexcept = default;

        constexpr reference operator*() const noexcept
        { return *current_element_; }

        constexpr pointer operator->() const noexcept
        { return current_element_; }

    protected:
        pointer current_element_{nullptr};
    };


    template<typename T>
    class dynamic_array
    {
    public:
        using value_type = T;

        using iterator = dynamic_array_iterator<value_type, false>;
        using const_iterator = dynamic_array_iterator<value_type, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr dynamic_array() = default;
        constexpr dynamic_array(const dynamic_array& other)
        {
            resize(other.size());
            std::copy_n(other.data_, size_, data_);
        }

        constexpr dynamic_array(dynamic_array&& other) noexcept
        {
            *this = std::move(other);
        }

        explicit constexpr dynamic_array(size_t n, value_type default_value = value_type{})
        {
            resize(n);
            std::fill(begin(), end(), default_value);
        }

        constexpr dynamic_array(std::initializer_list<value_type> init_list)
        {
            resize(init_list.size());
            std::copy_n(std::begin(init_list), size_, begin());
        }

        template<typename InputIterator>
        requires std::input_iterator<InputIterator>
        constexpr dynamic_array(InputIterator begin_it, InputIterator end_it)
        {
            resize(std::distance(begin_it, end_it));
            std::copy_n(begin_it, size_, begin());
        }

        dynamic_array& operator=(const dynamic_array& other)
        {
            if (this == &other)
                return *this;
            dynamic_array tmp(other);
            std::swap(data_, tmp.data_);
            std::swap(size_, tmp.size_);
            return *this;
        }

        dynamic_array& operator=(dynamic_array&& other) noexcept
        {
            if (this == &other)
                return *this;

            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;

            return *this;
        }

        constexpr value_type& operator[](size_t index)
        {
            if (index >= size_) 
                throw std::out_of_range("dynamic_array::operator[" 
                + std::to_string(index)+ "] out of range, array::size() is" 
                + std::to_string(size_));
            return data_[index];
        }
    
        constexpr const value_type& operator[](size_t index) const
        {
            if (index >= size_) 
                throw std::out_of_range("const dynamic_array::operator[" 
                + std::to_string(index)+ "] out of range, array::size() is" 
                + std::to_string(size_));
            return data_[index];
        }
        constexpr value_type& at(size_t index)
        { return this->operator[](index); }

        [[nodiscard]] constexpr const value_type& at(size_t index) const
        { return this->operator[](index); }

        [[nodiscard]] constexpr size_t size() const noexcept
        { return size_; }

        constexpr void resize(size_t new_size)
        {
            value_type* new_buff = nullptr;
            if (new_size != 0)
                new_buff = new value_type[new_size];
            if (data_ != nullptr && size_ != 0) 
            {
                std::copy_n(data_, std::min(size_, new_size), new_buff);
                delete[] data_;
            }
            size_ = new_size;
            data_ = new_buff;
        }

        constexpr void clear() noexcept
        { resize(0); }

        template<typename Container>
        requires std::ranges::range<Container>
        constexpr auto operator<=>(const Container& container) const noexcept
        { return compare_collections(*this, container); }

        template<typename Container>
        requires std::ranges::range<Container>
        constexpr bool operator==(const Container& container) const noexcept
        { return is_equal_collections(*this, container); }

        constexpr iterator begin() noexcept
        { return iterator{data_}; }
        constexpr iterator end() noexcept
        { return iterator{data_ + size_}; }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        { return const_iterator{data_}; }
        [[nodiscard]] constexpr const_iterator end() const noexcept
        { return const_iterator{data_ + size_}; }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        { return const_iterator{data_}; }
        [[nodiscard]] constexpr const_iterator cend() const noexcept
        { return const_iterator{data_ + size_}; }


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

        constexpr ~dynamic_array()
        { clear(); }

    private:
        value_type* data_ = nullptr;
        size_t size_ = size_t{0};
    };

    template<typename InputIterator>
    requires std::input_iterator<InputIterator>
    dynamic_array(InputIterator begin_it, InputIterator end_it)
    -> dynamic_array<typename std::iterator_traits<InputIterator>::value_type>;
}

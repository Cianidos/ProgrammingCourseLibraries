#pragma once

#include "dynamic_array.h"
#include "sequence.h"
#include <algorithm>
#include <numeric>
#include <ranges>
#include <vector>

namespace math_objects
{
    template<typename T, size_t N>
    class vector
    {
    public:
        using container = collections::dynamic_array<T>;
        using value_type = typename container::value_type;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
        using reverse_iterator = typename container::reverse_iterator;
        using const_reverse_iterator = typename container::const_reverse_iterator;

    private:
        using sequence = collections::sequence<container>;
        sequence data_ = container(N);

    public:
        constexpr vector() = default;
        constexpr vector(const vector& other) = default;
        constexpr vector(vector&& other) = default;

        constexpr explicit vector(const sequence& data)
            : data_(data) { }


        constexpr size_t size() const
        { return N; }

        constexpr T& operator[](size_t index)
        {
            return data_[index];
        }

        constexpr const T& operator[](size_t index) const
        {
            return data_[index];
        }


        constexpr vector operator+ (const vector& rhs) const
        {
            vector result = *this;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] += rhs[i];
            }
            return result;
        }
    
        constexpr vector operator- (const vector& rhs) const
        {
            vector result = *this;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] -= rhs[i];
            }
            return result;
        }

        constexpr T operator* (const vector& rhs) const
        {
            T _sum{};
            _sum *= 0;
            for (size_t i = 0; i < N; ++i)
            {
                _sum += (*this)[i] * rhs[i];
            }
            return _sum;
        }

        constexpr vector operator* (const T& rhs)
        {
            vector result;
            std::transform(std::begin(*this), std::end(*this), std::begin(result), [&rhs](T& x) { return x * rhs; });
            return result;
        }

        constexpr vector operator/ (const T& rhs)
        {
            vector result;
            std::transform(std::begin(*this), std::end(*this), std::begin(result), [&rhs](T& x) { return x / rhs; });
            return result;
        }

        constexpr iterator begin()
        { return std::begin(data_); }
        constexpr iterator end()
        { return std::end(data_); }
        constexpr const_iterator begin() const
        { return std::begin(data_); }
        constexpr const_iterator end() const
        { return std::end(data_); }
        constexpr const_iterator cbegin() const
        { return std::cbegin(data_); }
        constexpr const_iterator cend() const
        { return std::cend(data_); }
        constexpr reverse_iterator rbegin()
        { return std::rbegin(data_); }
        constexpr reverse_iterator rend()
        { return std::rend(data_); }
        constexpr const_reverse_iterator rbegin() const
        { return std::rbegin(data_); }
        constexpr const_reverse_iterator rend() const
        { return std::rend(data_); }
        constexpr const_reverse_iterator crbegin() const
        { return std::crbegin(data_); }
        constexpr const_reverse_iterator crend() const
        { return std::crend(data_); }
    };
}

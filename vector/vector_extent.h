#ifndef MULTIVECTOR_EXTENT_H
#define MULTIVECTOR_EXTEND_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace vec::detail {

template <typename It1, typename It2>
inline constexpr void const_copy(It1 from, It1 to, It2 dst)
{
    for (; from != to; ++from, ++dst) { *dst = *from; }
}


template <typename T, std::size_t Dimensions>
class extent final
{
public:
    using value_type = extent<T, Dimensions-1>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
//     using iterator = extent_iterator<T, Dimensions-1>;
//     using const_iterator = const_extent_iterator<T, Dimensions-1>;
    static constexpr std::size_t dimensions = Dimensions;
    using shape_type = std::size_t[Dimensions];

public:
    extent(const extent &) = delete;

    template <typename U, typename = std::enable_if_t<std::is_integral_v<U>>>
    constexpr extent(T * ptr, const U (&shape)[Dimensions])
     : m_data(ptr)
    {
//         assert(std::none_of(std::begin(shape), std::end(shape), // we want to allow int for convenience
//                             [](auto val) { return val < 0; })); // negative is UB, assert on debug
        const_copy(std::begin(shape), std::end(shape), std::begin(m_shape));
    }
private:
    constexpr extent(T * ptr, const std::size_t * shape)
     : m_data(ptr)
    {
        std::copy_n(shape, Dimensions, m_shape);
    }
public:

    extent & operator=(const extent & other)
    {
        assert(m_shape == other.m_shape);
        std::copy(other.m_data, other.m_data + flat_size(), m_data);
        return *this;
    }

    constexpr size_type size() const { return m_shape[0]; }
    constexpr size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    constexpr bool empty() const { return false; }

    extent & operator=(std::initializer_list<T> values)
    {
        this->assign(values);
        return *this;
    }

    void assign(std::initializer_list<T> values)
    {
        assert(flat_size() == values.size());
        std::copy(values.begin(), values.end(), m_data);
    }

    value_type at(size_type pos)
    {
        if (!(pos < m_shape[0])) { throw std::out_of_range("index out of range"); }
        return (*this)[pos];
    }

    const value_type at(size_type pos) const
    {
        if (!(pos < m_shape[0])) { throw std::out_of_range("index out of range"); }
        return (*this)[pos];
    }

    constexpr value_type operator[](size_type pos)
    {
        auto extentSize = size_type{1};
        for (std::size_t idx = Dimensions - 1; idx > 0; --idx) { extentSize *= m_shape[idx]; }
        return value_type(m_data + extentSize * pos, &m_shape[1]);
    }

    constexpr const value_type operator[](size_type pos) const
    {
        auto extentSize = size_type{1};
        for (std::size_t idx = Dimensions - 1; idx > 0; --idx) { extentSize *= m_shape[idx]; }
        return value_type(m_data + extentSize * pos, &m_shape[1]);
    }

    constexpr T * data() noexcept { return m_data; }
    constexpr const T * data() const noexcept { return m_data; }

    constexpr const shape_type & shape() const noexcept { return m_shape; }

    constexpr size_type flat_size() const {
        auto size = m_shape[0]; // will be skipped by loop
        for (std::size_t idx = Dimensions - 1; idx > 0; --idx) { size *= m_shape[idx]; }
        return size;
    }

private:
    T *         m_data;
    shape_type  m_shape = {};

    friend class extent<T, Dimensions+1>;
};

template <typename T, std::size_t Dimensions>
constexpr bool operator==(const extent<T, Dimensions> & lhs, const extent<T, Dimensions> & rhs)
{
    return std::equal(std::begin(lhs.shape()), std::end(lhs.shape()), std::begin(rhs.shape())) &&
           std::equal(lhs.data(), lhs.data() + lhs.flat_size(), rhs.data());
}

template <typename T, std::size_t Dimensions>
constexpr bool operator!=(const extent<T, Dimensions> & lhs, const extent<T, Dimensions> & rhs)
{
    return !(lhs == rhs);
}

// Compare 1-dimension extent to regular vector
template <typename T, typename Alloc>
bool operator==(const extent<T, 1u> & lhs, const std::vector<T, Alloc> & rhs)
{
    return std::equal(lhs.data(), lhs.data() + lhs.size(), rhs.begin(), rhs.end());
}
template <typename T, typename Alloc>
bool operator==(const std::vector<T, Alloc> & lhs, const extent<T, 1u> & rhs) { return rhs == lhs; }
template <typename T, typename Alloc>
bool operator!=(const extent<T, 1u> & lhs, const std::vector<T, Alloc> & rhs) { return !(lhs == rhs); }
template <typename T, typename Alloc>
bool operator!=(const std::vector<T, Alloc> & lhs, const extent<T, 1u> & rhs) { return !(rhs == lhs); }


template <typename T>
class extent<T, 1u> final
{
public:
    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = T *;
    using const_iterator = T const *;
    static constexpr std::size_t dimensions = 1u;
    using shape_type = std::size_t[1];

public:
    extent(const extent &) = delete;

    template <typename U, typename = std::enable_if_t<std::is_integral_v<U>>>
    constexpr extent(T * ptr, const U (&shape)[1u])
     : m_data(ptr), m_shape{shape[0]}
    {
        assert(shape[0] >= 0);
    }
private:
    constexpr extent(T * ptr, const std::size_t * shape)
     : m_data(ptr), m_shape{shape[0]}
    { }
public:

    extent & operator=(const extent & other)
    {
        assert(m_shape == other.m_shape);
        std::copy(other.m_data, other.m_data + flat_size(), m_data);
        return *this;
    }

    iterator begin() { return m_data; }
    const_iterator begin() const { return m_data; }
    const_iterator cbegin() const { return m_data; }
    iterator end() { return m_data + m_shape[0]; }
    const_iterator end() const { return m_data + m_shape[0]; }
    const_iterator cend() const { return m_data + m_shape[0]; }

    constexpr size_type size() const { return m_shape[0]; }
    constexpr size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    constexpr bool empty() const { return false; }

    extent & operator=(std::initializer_list<T> values)
    {
        this->assign(values);
        return *this;
    }

    void assign(std::initializer_list<T> values)
    {
        assert(flat_size() == values.size());
        std::copy(values.begin(), values.end(), m_data);
    }

    constexpr reference front() { return m_data[0]; }
    constexpr const_reference front() const { return m_data[0]; }
    constexpr reference back() { return m_data[m_shape[0] - 1]; }
    constexpr const_reference back() const { return m_data[m_shape[0] - 1]; }

    reference at(size_type pos)
    {
        if (!(pos < m_shape[0])) { throw std::out_of_range("index out of range"); }
        return (*this)[pos];
    }

    const_reference at(size_type pos) const
    {
        if (!(pos < m_shape[0])) { throw std::out_of_range("index out of range"); }
        return (*this)[pos];
    }

    constexpr reference operator[](size_type pos) { return m_data[pos]; }
    constexpr const_reference operator[](size_type pos) const { return m_data[pos]; }

    constexpr T * data() noexcept { return m_data; }
    constexpr const T * data() const noexcept { return m_data; }

    constexpr const shape_type & shape() const noexcept { return m_shape; }
    constexpr size_type flat_size() const { return m_shape[0]; }

private:
    T *         m_data;
    shape_type  m_shape = {};

    friend class extent<T, 2u>;
};


} // namespace vec::detail

#endif

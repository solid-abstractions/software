#include "vector_extent.h"

#include <gtest/gtest.h>
#include <numeric>
#include <vector>

using vec::detail::extent;


TEST(extent, requirements)
{
    using type = extent<int, 3>;

    // Container requirements
    static_assert(std::is_same_v<type::value_type, extent<int, 2>>);
    static_assert(std::is_same_v<type::value_type::value_type, extent<int, 1>>);
    static_assert(std::is_same_v<type::value_type::value_type::value_type, int>);
    // iterator is iterator to T
    // const_iterator is const iterator to T
    // reverse_iterator
    // const_reverse_iterator
    static_assert(std::is_same_v<typename type::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<typename type::size_type, std::size_t>);
    static_assert(type::dimensions == 3);

    static_assert(!std::is_default_constructible_v<type>);
    static_assert(!std::is_copy_constructible_v<type>);
    static_assert(std::is_copy_assignable_v<type>);
    static_assert(std::is_destructible_v<type>);

    SUCCEED();
}

TEST(extent, basic)
{
    int data[3*4] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto obj = extent(data, {3u, 4u});

    EXPECT_EQ(obj.size(), 3);
    EXPECT_EQ(obj.max_size(), (std::numeric_limits<extent<int, 2>::size_type>::max()));
    EXPECT_FALSE(obj.empty());

    EXPECT_EQ(obj.data(), &data[0]);
    EXPECT_TRUE(std::equal(std::begin(obj.shape()), std::end(obj.shape()), std::array{3u, 4u}.begin()));
    EXPECT_EQ(obj.flat_size(), 12);
}

TEST(extent, subextent)
{
    int data[3*4] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto obj = extent(data, {3u, 4u});

    for (std::size_t y = 0; y < 3; ++y) {
        EXPECT_NO_THROW(obj.at(y));
        auto row = obj[y];
        EXPECT_EQ(row.size(), 4);
        EXPECT_FALSE(row.empty());
        EXPECT_EQ(row.data(), &data[4 * y]);
        EXPECT_TRUE(std::equal(std::begin(row.shape()), std::end(row.shape()), std::array{4}.begin()));
        EXPECT_EQ(row.flat_size(), 4);
    }
    EXPECT_THROW(obj.at(3), std::out_of_range);
}

TEST(extent, finalitems)
{
    int data[3*4] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto obj = extent(data, {3u, 4u});

    int sum = 0;
    for (std::size_t y = 0; y < 3; ++y) {
        for (std::size_t x = 0; x < 4; ++x) {
            sum += obj[y][x];
        }
    }
    EXPECT_EQ(sum, 66);

    for (std::size_t y = 0; y < 3; ++y) {
        for (std::size_t x = 0; x < 4; ++x) {
            EXPECT_NO_THROW(obj[y].at(x));
            EXPECT_EQ(obj[y][x], 4*y + x) <<"at item [" <<y <<"][" <<x <<']';
        }
        EXPECT_THROW(obj[y].at(4), std::out_of_range);
    }
}

TEST(extent, compare)
{
    int data[3*4] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto objA = extent(data, {3u, 4u});
    auto objB = extent(data, {3u, 4u});
    auto objC = extent(data, {4u, 3u});

    EXPECT_TRUE(objA == objB);
    EXPECT_FALSE(objA != objB);
    EXPECT_FALSE(objA  == objC);
    EXPECT_TRUE(objA != objC);

    EXPECT_FALSE(objA[0] == objA[1]);
    EXPECT_TRUE(objA[0] != objA[1]);
    EXPECT_TRUE(objA[0] == (std::vector{0, 1, 2, 3}));
    EXPECT_FALSE(objA[0] != (std::vector{0, 1, 2, 3}));
    EXPECT_TRUE((std::vector{0, 1, 2, 3}) == objA[0]);
    EXPECT_FALSE((std::vector{0, 1, 2, 3}) != objA[0]);
}

TEST(extent, assign)
{
    int dataA[3*4] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22};
    int dataB[3*4] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23};
    auto objA = extent(dataA, {3u, 4u});
    auto objB = extent(dataB, {3u, 4u});

    objA = objB;
    EXPECT_TRUE(std::equal(std::begin(dataA), std::end(dataA), std::begin(dataB)));

    objA[0] = objA[2];
    EXPECT_EQ(objA[0], objA[2]);


    objA[1] = {42, 42, 42, 42};
    EXPECT_EQ(objA[1], (std::vector{42, 42, 42, 42}));

    int dataFinal[3*4] = {17, 19, 21, 23, 42, 42, 42, 42, 17, 19, 21, 23};
    EXPECT_TRUE(std::equal(std::begin(dataA), std::end(dataA), std::begin(dataFinal)));
}

TEST(extent, compile)
{
    static constexpr int data[3*4] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22};
    constexpr auto obj = extent(data, {3u, 4u});

    static_assert(obj.size() == 3);
    static_assert(!obj.empty());
    static_assert(obj[1].size() == 4);
    static_assert(obj[1][1] == 10);

    SUCCEED();
}



//SequenceContainer ->
// X(n, t)  n copies of t
// X(it, it) copy range
// X(initializer_list)
// = initializer_list

//Container ->
// begin() end()
// cbegin cend()
// == !=
// swap member, swap external
// size max_isze empty

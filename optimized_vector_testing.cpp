#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include <gtest/gtest.h>

#include "optimized_vector.h"

const size_t BIG_SIZE = 1000;
const size_t SMALL_SIZE = 1;
const uint32_t VAL = 42;

TEST(vector, default_ctor)
{
    optimized_vector v;
    // No throw expected
    ASSERT_TRUE(true);
}

TEST(vector, size_t_ctor)
{
    const size_t N = 60;
    optimized_vector v(N);
    // No throw expected
    for (uint32_t i = 0; i < N; ++i)
    {
        v[i] = i;
    }
    for (uint32_t i = 0; i < N; ++i)
    {
        ASSERT_EQ(v[i], i);
    }
}

TEST(vector, size_t_and_val_ctor)
{
    const size_t N = 60;
    const uint32_t VAL = 42;
    optimized_vector v(N, VAL);
    // No throw expected
    for (uint32_t i = 0; i < N; ++i)
    {
        ASSERT_EQ(v[i], VAL);
    }
}

TEST(vector, small_copy_ctor)
{
    const size_t N = 1;
    const uint32_t VAL = 42;
    optimized_vector v(N, VAL);
    for (size_t i = 0; i < N; i++)
        v[i] += i;

    optimized_vector vv(v);
    for (size_t i = 0; i < N; i++)
        ASSERT_EQ(vv[i], VAL + i);
}


TEST(vector, big_copy_ctor)
{
    const size_t N = 1000;
    const uint32_t VAL = 42;
    optimized_vector v(N, VAL);
    for (size_t i = 0; i < N; i++)
        v[i] += i;

    optimized_vector vv(v);
    for (size_t i = 0; i < N; i++)
        ASSERT_EQ(vv[i], VAL + i);
}

TEST(vector, initializer_list_ctor)
{
    optimized_vector v{1, 2, 3, 4, 5};
    for (uint32_t i = 0; i < 4; ++i)
        ASSERT_EQ(v[i], i + 1);
}

TEST(vector, initializer_list_arg_ctor)
{
    optimized_vector v({1, 2, 3, 4, 5});
    for (uint32_t i = 0; i < 4; ++i)
        ASSERT_EQ(v[i], i + 1);
}

TEST(vector, small_detaching)
{
    optimized_vector v(SMALL_SIZE, VAL);
    optimized_vector vv(v);

    for (size_t i = 0; i < SMALL_SIZE; ++i)
        vv[i] += i;

    for (size_t i = 0; i < SMALL_SIZE; ++i)
        ASSERT_EQ(v[i], VAL);
    for (size_t i = 0; i < SMALL_SIZE; ++i)
        ASSERT_EQ(vv[i], VAL + i);
}

TEST(vector, big_detaching)
{
    optimized_vector v(BIG_SIZE, VAL);
    optimized_vector vv(v);

    for (size_t i = 0; i < BIG_SIZE; ++i)
        vv[i] += i;

    for (size_t i = 0; i < BIG_SIZE; ++i)
        ASSERT_EQ(v[i], VAL);
    for (size_t i = 0; i < BIG_SIZE; ++i)
        ASSERT_EQ(vv[i], VAL + i);
}

TEST(vector, getting_size)
{
    const size_t SIZE = 61;
    optimized_vector v(SIZE);
    ASSERT_EQ(v.size(), SIZE);
}

TEST(vector, small_push_back)
{
    optimized_vector v;
    v.push_back(VAL);
    ASSERT_EQ(v[0], VAL);
}

TEST(vector, big_push_back)
{
    optimized_vector v(BIG_SIZE);
    v.push_back(VAL);
    ASSERT_EQ(v[BIG_SIZE], VAL);
}

TEST(vector, small_to_big_push_back)
{
    optimized_vector v;
    for (size_t i = 0; i < BIG_SIZE; ++i)
        v.push_back(i);
    for (size_t i = 0; i < BIG_SIZE; ++i)
        ASSERT_EQ(v[i], i);
}

TEST(vector, changing_size)
{
    optimized_vector v;
    for (size_t i = 0; i < BIG_SIZE; ++i)
    {
        ASSERT_EQ(v.size(), i);
        v.push_back(i);
    }
}

TEST(vector, small_small_assignment)
{
    optimized_vector a(SMALL_SIZE, 1);
    optimized_vector b(SMALL_SIZE, 2);
    b = a;
    for (size_t i = 0; i < a.size(); ++i)
        ASSERT_EQ(b[i], a[i]);
}

TEST(vector, big_big_assignment)
{
    optimized_vector a(BIG_SIZE, 1);
    optimized_vector b(BIG_SIZE, 2);
    b = a;
    for (size_t i = 0; i < a.size(); ++i)
        ASSERT_EQ(b[i], a[i]);
}

TEST(vector, big_small_assignment)
{
    optimized_vector a(SMALL_SIZE, 1);
    optimized_vector b(BIG_SIZE, 2);
    b = a;
    for (size_t i = 0; i < a.size(); ++i)
        ASSERT_EQ(b[i], a[i]);
}

TEST(vector, small_big_assignment)
{
    optimized_vector a(BIG_SIZE, 1);
    optimized_vector b(SMALL_SIZE, 2);
    b = a;
    for (size_t i = 0; i < a.size(); ++i)
        ASSERT_EQ(b[i], a[i]);
}

TEST(vector, small_change_after_assignment)
{
    optimized_vector a(SMALL_SIZE, 1);
    optimized_vector b(SMALL_SIZE, 2);
    b = a;
    b[0] = 5;
    ASSERT_EQ(a[0], 1u);
    ASSERT_EQ(b[0], 5u);
}

TEST(vector, big_change_after_assignment)
{
    optimized_vector a(BIG_SIZE, 1);
    optimized_vector b(BIG_SIZE, 2);
    b = a;
    b[0] = 5;
    ASSERT_EQ(a[0], 1u);
    ASSERT_EQ(b[0], 5u);
}

TEST(vector, swap)
{
    optimized_vector a(BIG_SIZE, 1);
    optimized_vector b(SMALL_SIZE, 2);
    optimized_vector c(a), d(b);

    swap(a, b);

    for (size_t i = 0; i < b.size(); ++i)
        ASSERT_EQ(b[i], c[i]);
    for (size_t i = 0; i < a.size(); ++i)
        ASSERT_EQ(a[i], d[i]);
}

TEST(vector, back)
{
    optimized_vector a(BIG_SIZE), b(SMALL_SIZE);
    a.back() = 7;
    ASSERT_EQ(a.back(), 7u);
    b.back() = 8;
    ASSERT_EQ(b.back(), 8u);
}

TEST(vector, small_pop_back)
{
    optimized_vector a(SMALL_SIZE);
    a.pop_back();
    ASSERT_EQ(a.size(), SMALL_SIZE - 1);
}

TEST(vector, big_pop_back)
{
    optimized_vector a(BIG_SIZE);
    a.back() = 7;
    a[a.size() - 2] = 6;
    a.pop_back();
    ASSERT_EQ(a.back(), 6);
    ASSERT_EQ(a.size(), BIG_SIZE - 1);
}

TEST(vector, big_to_small_pop_back)
{
    optimized_vector a(BIG_SIZE);
    for (uint32_t i = 0; i < BIG_SIZE; ++i)
        a[i] = i + 1;
    while (a.size() > SMALL_SIZE)
        a.pop_back();
    ASSERT_EQ(a.size(), SMALL_SIZE);
    ASSERT_EQ(a[0], 1);
}
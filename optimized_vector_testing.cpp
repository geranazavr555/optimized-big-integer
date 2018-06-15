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
//
// Created by georgiy on 14.06.18.
//

#include "optimized_vector.h"

using std::make_shared;
using std::shared_ptr;

// ===================================== big_vector ============================================================


optimized_vector::big_vector::big_vector(size_t capacity, shared_ptr<uint32_t> data) :
        capacity(capacity), data(std::move(data))
{}

const uint32_t* optimized_vector::big_vector::begin() const
{
    return data.get();
}

uint32_t* optimized_vector::big_vector::begin()
{
    return data.get();
}

// TODO : шота кек
const uint32_t* optimized_vector::big_vector::end() const
{
    return data.get() + capacity;
}

uint32_t* optimized_vector::big_vector::end()
{
    return data.get() + capacity;
}

const uint32_t& optimized_vector::big_vector::operator[](size_t idx) const
{
    return data.get()[idx];
}

uint32_t& optimized_vector::big_vector::operator[](size_t idx)
{
    return data.get()[idx];
}

void optimized_vector::big_vector::detach(size_t useful_size)
{
    if (data.unique())
        return;

    static const size_t OVERSIZE = 2;
    capacity = useful_size + OVERSIZE;

    auto new_data = new uint32_t[capacity];
    std::copy(begin(), begin() + useful_size, new_data);
    data.reset(new_data);
}

// ===================================== optimized_vector ======================================================


optimized_vector::optimized_vector() :
        size(0)
{}

optimized_vector::optimized_vector(size_t n) :
        size(n)
{
    if (is_small())
        for (size_t i = 0; i < n; ++i)
            small_data[i] = 0;
    else
    {
        new(&data) big_vector(n, shared_ptr<uint32_t>(new uint32_t[n], std::default_delete<uint32_t[]>()));
    }
}

optimized_vector::optimized_vector(size_t n, uint32_t val) :
        size(n)
{
    if (is_small())
        for (size_t i = 0; i < n; ++i)
            small_data[i] = val;
    else
    {
        new(&data) big_vector(n, shared_ptr<uint32_t>(new uint32_t[n], std::default_delete<uint32_t[]>()));
        for (size_t i = 0; i < n; ++i)
            data.begin()[i] = val;
    }
}

optimized_vector::optimized_vector(optimized_vector const &other) :
        size(other.size)
{
    if (is_small())
        std::copy(other.small_data, other.small_data + SMALL_OBJECT_SIZE, small_data);
    else
    {
        new(&data) big_vector(other.data.capacity, other.data.data);
    }
}

optimized_vector::~optimized_vector()
{
    if (!is_small())
    {
        data.~big_vector();
    }
}

bool optimized_vector::is_small() const
{
    return size <= SMALL_OBJECT_SIZE;
}

const uint32_t& optimized_vector::operator[](size_t idx) const
{
    return (is_small() ? small_data[idx] : data[idx]);
}

uint32_t& optimized_vector::operator[](size_t idx)
{
    detach();
    if (is_small())
        return small_data[idx];
    return data[idx];
}

void optimized_vector::detach()
{
    if (!is_small())
        data.detach(size);
}
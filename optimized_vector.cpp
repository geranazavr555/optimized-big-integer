//
// Created by georgiy on 14.06.18.
//

#include "optimized_vector.h"
#include <cstring>

using std::make_shared;
using std::shared_ptr;

// ===================================== big_vector ============================================================


optimized_vector::big_vector::big_vector(size_t capacity, shared_ptr<uint32_t> data) :
        capacity(capacity), data(std::move(data))
{}

optimized_vector::big_vector::big_vector(big_vector const &other) :
        capacity(other.capacity),
        data(other.data)
{}

const uint32_t* optimized_vector::big_vector::begin() const
{
    return data.get();
}

uint32_t* optimized_vector::big_vector::begin()
{
    return data.get();
}

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
    data.reset(new_data, std::default_delete<uint32_t[]>());
}

void optimized_vector::big_vector::guarantee_capacity(size_t cap)
{
    if (cap > capacity)
    {
        size_t new_capacity = std::max(cap + 2, capacity * 2);
        auto new_data = new uint32_t[new_capacity];
        std::copy(begin(), end(), new_data);

        capacity = new_capacity;
        data.reset(new_data, std::default_delete<uint32_t[]>());
    }
}

// ===================================== optimized_vector ======================================================


optimized_vector::optimized_vector() :
        siz(0)
{}

optimized_vector::optimized_vector(size_t n) :
        siz(n)
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
        siz(n)
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
        siz(other.siz)
{
    if (is_small())
        std::copy(other.small_data, other.small_data + SMALL_OBJECT_SIZE, small_data);
    else
    {
        new(&data) big_vector(other.data.capacity, other.data.data);
    }
}

optimized_vector::optimized_vector(std::initializer_list<uint32_t> init_data) : siz(init_data.size())
{
    if (is_small())
        std::copy(init_data.begin(), init_data.end(), small_data);
    else
    {
        new(&data) big_vector(init_data.size(),
                              shared_ptr<uint32_t>(new uint32_t[init_data.size()], std::default_delete<uint32_t[]>()));
        std::copy(init_data.begin(), init_data.end(), data.begin());
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
    return siz <= SMALL_OBJECT_SIZE;
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

void optimized_vector::swap(optimized_vector &other) noexcept
{
    char tmp[sizeof(optimized_vector)];
    memcpy(&tmp, this, sizeof(optimized_vector));
    memcpy(this, &other, sizeof(optimized_vector));
    memcpy(&other, &tmp, sizeof(optimized_vector));
}

void swap(optimized_vector &a, optimized_vector &b)
{
    a.swap(b);
}

optimized_vector& optimized_vector::operator=(optimized_vector const &other)
{
    optimized_vector tmp(other);
    swap(tmp);
    return *this;
}

void optimized_vector::detach()
{
    if (!is_small())
        data.detach(siz);
}

size_t optimized_vector::size() const
{
    return siz;
}

void optimized_vector::to_big()
{
    if (is_small())
    {
        size_t capacity = std::max(SMALL_OBJECT_SIZE + 2, siz * 2);

        shared_ptr<uint32_t> ptr(new uint32_t[capacity], std::default_delete<uint32_t[]>());
        std::copy(small_data, small_data + siz, ptr.get());
        new(&data) big_vector(capacity, ptr);
    }
}

void optimized_vector::push_back(uint32_t const &val)
{
    if (siz < SMALL_OBJECT_SIZE)
    {
        small_data[siz] = val;
        ++siz;
        return;
    }
    if (siz == SMALL_OBJECT_SIZE)
        to_big();
    data.guarantee_capacity(siz + 1);
    data[siz] = val;
    ++siz;
}

const uint32_t& optimized_vector::back() const
{
    if (siz == 0)
        throw std::runtime_error("back() is not allowed in empty vector");
    if (is_small())
        return small_data[siz - 1];
    return data[siz - 1];
}

uint32_t& optimized_vector::back()
{
    if (siz == 0)
        throw std::runtime_error("back() is not allowed in empty vector");
    if (is_small())
        return small_data[siz - 1];
    return data[siz - 1];
}
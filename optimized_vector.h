#ifndef OPTIMIZED_VECTOR_H
#define OPTIMIZED_VECTOR_H

#include <cstdint>
#include <cstddef>
#include <memory>

class optimized_vector {
    class big_vector
    {
    public:
        size_t capacity;
        std::shared_ptr <uint32_t> data;

    public:
        big_vector() = default;
        big_vector(size_t capacity, std::shared_ptr<uint32_t> data);
        big_vector(big_vector const &other);

        uint32_t* begin();
        const uint32_t* begin() const;
        uint32_t* end();
        const uint32_t* end() const;

        const uint32_t& operator[](size_t idx) const;
        uint32_t& operator[](size_t idx);

        //makes object unique
        void detach(size_t useful_data);

        void guarantee_capacity(size_t cap);
    };

    static const size_t SMALL_OBJECT_SIZE = sizeof(big_vector) / sizeof(uint32_t);

    size_t siz;
    union
    {
        uint32_t small_data[SMALL_OBJECT_SIZE];
        big_vector data;
    };

private:
    bool is_small() const;
    void to_big();
    void to_small();

public:
    optimized_vector();
    optimized_vector(size_t n);
    optimized_vector(size_t n, uint32_t val);

    optimized_vector(optimized_vector const& other);
    optimized_vector(std::initializer_list<uint32_t> data);

    ~optimized_vector();

    const uint32_t& operator[](size_t idx) const;
    uint32_t& operator[](size_t idx);

    optimized_vector& operator=(optimized_vector const &other);
    void swap(optimized_vector& other) noexcept;

    void push_back(uint32_t const &val);
    void pop_back();
    const uint32_t& back() const;
    uint32_t& back();

    size_t size() const;
    void detach();
};

void swap(optimized_vector &a, optimized_vector& b);

#endif //OPTIMIZED_VECTOR_H

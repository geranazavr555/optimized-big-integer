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
        big_vector() = default; // WHAT?
        big_vector(size_t capacity, std::shared_ptr<uint32_t> data);

        uint32_t* begin();
        const uint32_t* begin() const;
        uint32_t* end();
        const uint32_t* end() const;

        const uint32_t& operator[](size_t idx) const;
        uint32_t& operator[](size_t idx);

        //makes object unique
        void detach(size_t useful_data);

    };

    static const size_t SMALL_OBJECT_SIZE = sizeof(big_vector) / sizeof(uint32_t);

    size_t size;
    union
    {
        uint32_t small_data[SMALL_OBJECT_SIZE];
        big_vector data;

       // TODO : constructor, destructor
    };

private:
    bool is_small() const;

public:
    optimized_vector();
    optimized_vector(size_t n);
    optimized_vector(size_t n, uint32_t val);

    optimized_vector(optimized_vector const& other);

    ~optimized_vector();

    const uint32_t& operator[](size_t idx) const;
    uint32_t& operator[](size_t idx);

    void detach();
};


#endif //OPTIMIZED_VECTOR_H

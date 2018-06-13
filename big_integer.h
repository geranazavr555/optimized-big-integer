#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <cstdint>
#include <string>
#include <ostream>

class big_integer
{
    static const uint64_t BASE = (1ull << 32);
    static const uint32_t LOG_BASE = 32u;

    bool sign;
    std::vector <uint32_t> number;
    size_t first_non_zero_index;
    bool is_two_complemented;

    explicit big_integer(std::vector<uint32_t> const& number, bool sign = false, bool two_complemented = false);
    explicit big_integer(uint32_t x);

    size_t size() const;
    void normalize();

    std::pair<big_integer, uint32_t> divide_by_short(uint32_t x, bool sign=false);

    uint32_t digit_in_abs_format(size_t n) const;
    uint32_t digit_in_twos_complement(size_t n) const;

public:
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;

    big_integer& operator=(big_integer const& other);

    friend big_integer operator+(big_integer a, big_integer const& b);
    friend big_integer operator-(big_integer a, big_integer const& b);
    friend big_integer operator*(big_integer a, big_integer const& b);
    friend big_integer operator/(big_integer a, big_integer const& b);
    friend big_integer operator%(big_integer a, big_integer const& b);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    friend big_integer operator&(big_integer a, big_integer const& b);
    friend big_integer operator|(big_integer a, big_integer const& b);
    friend big_integer operator^(big_integer a, big_integer const& b);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    friend big_integer operator<<(big_integer a, int b);
    friend big_integer operator>>(big_integer a, int b);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend void emplace_shl(std::vector<uint32_t> const &src, int b, std::vector<uint32_t> &dest);
    friend void emplace_shr(std::vector<uint32_t> const &src, int b, std::vector<uint32_t> &dest);

    friend std::string to_string(big_integer const& a);
    friend big_integer from_string(std::string const& str);
    friend big_integer abs(big_integer const& x);

    void swap(big_integer &other);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
big_integer from_string(std::string const& str);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

big_integer abs(big_integer const& x);

#endif // BIG_INTEGER_H

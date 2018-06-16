#include "big_integer.h"

#include <algorithm>
#include <iostream>

typedef optimized_vector vector;

big_integer::big_integer() : sign(false),
                             number(1, 0),
                             first_non_zero_index(SIZE_MAX),
                             is_two_complemented(false) {}

big_integer::big_integer(big_integer const& other) : sign(other.sign),
                                                     number(other.number),
                                                     first_non_zero_index(other.first_non_zero_index),
                                                     is_two_complemented(other.is_two_complemented)
{
    this->normalize();
}

big_integer::big_integer(int a) : sign(a < 0),
                                  number(),
                                  is_two_complemented(false)
{
    number.push_back(a >= 0 ? static_cast<uint32_t>(a) : ~static_cast<uint32_t>(a) + 1);
    first_non_zero_index = 0;
}


big_integer::big_integer(optimized_vector const& number, bool sign, bool two_complemented) :
                                                sign(sign), number(number), is_two_complemented(two_complemented)
{
    this->normalize();
}


big_integer::big_integer(uint32_t x): sign(false), number({x}), first_non_zero_index(x ? 0 : SIZE_MAX),
                                      is_two_complemented(false)
{
    //this->normalize();
}

inline uint32_t big_integer::digit_in_abs_format(size_t n) const
{
    if (n >= size())
        return 0;
    return number[n];
}

inline uint32_t big_integer::digit_in_twos_complement(size_t n) const
{
    if (first_non_zero_index > n)
        return 0;
    if (n >= size())
        return (sign ? UINT32_MAX : 0);
    if (first_non_zero_index < n)
        return (sign ? ~number[n] : number[n]);
    if (!sign)
        return number[n];

    // have to keep first non-zero and all less not inverted;
    uint mask = 0;
    uint32_t i = 0;
    while (!(number[n] & (1u << i))) {
        mask ^= (1u << i);
        ++i;
    }
    mask = ~(mask ^ (1u << i));
    return number[n] ^ mask;
}

big_integer from_string(std::string const& str)
{
    big_integer result;

    for (size_t i = (str[0] == '-' ? 1 : 0); i < str.size(); i++)
    {
        result *= 10;
        result += str[i] - '0';
    }
    if (str[0] == '-')
        result.sign = true;
    result.normalize();
    return result;
}

big_integer::big_integer(std::string const& str) : big_integer(from_string(str)) {}

void big_integer::normalize()
{
    if (is_two_complemented && sign)
    {
        // have to translate to abs format
        size_t i = 0;
        for (; i < size(); ++i)
        {
            if (number[i])
                break;
        }
        if (i == size())
        {
            number = vector(1, 0);
            sign = false;
            is_two_complemented = false;
            first_non_zero_index = SIZE_MAX;
            return;
        }

        uint mask = 0;
        uint32_t j = 0;
        while (!(number[i] & (1u << j)))
        {
            mask ^= (1u << j);
            ++j;
        }
        mask = ~(mask ^ (1u << j));
        number[i] ^= mask;

        for (++i; i < size(); ++i)
            number[i] = ~number[i];
    }


    while (number.size() > 1 && number.back() == 0)
        number.pop_back();

    first_non_zero_index = SIZE_MAX;
    for (size_t i = 0; i < size(); ++i)
    {
        if (number[i])
        {
            first_non_zero_index = i;
            break;
        }
    }
    if (first_non_zero_index == SIZE_MAX)
    {
        number = optimized_vector(1, 0);
        sign = false;
        is_two_complemented = false;
        first_non_zero_index = SIZE_MAX;
        return;
    }
    is_two_complemented = false;
}

void big_integer::swap(big_integer &other)
{
    std::swap(sign, other.sign);
    std::swap(number, other.number);
    std::swap(first_non_zero_index, other.first_non_zero_index);
    std::swap(is_two_complemented, other.is_two_complemented);
}

size_t big_integer::size() const
{
    return number.size();
}

big_integer& big_integer::operator=(big_integer const &other)
{
    big_integer tmp(other);
    swap(tmp);
    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    big_integer tmp(*this);
    tmp.sign = !tmp.sign;
    tmp.normalize();
    return tmp;
}

big_integer abs(big_integer const& x)
{
    big_integer result(x);
    result.sign = false;
    return result;
}

big_integer operator+(big_integer a, const big_integer &b)
{
    if (!a.sign && b.sign)
        return a - abs(b);
    if (a.sign && !b.sign)
        return b - abs(a);

    size_t max_len = std::max(a.size(), b.size());
    optimized_vector result(max_len + 1);

    size_t i = 0;
    uint64_t tmp = 0, carry = 0;
    for (; i < max_len; ++i)
    {
        uint64_t x = a.digit_in_abs_format(i);
        uint64_t y = b.digit_in_abs_format(i);
        if (x < y)
            x += carry;
        else
            y += carry;
        tmp = x + y;
        result[i] = static_cast<uint32_t>(tmp);
        carry = tmp >> big_integer::LOG_BASE;
    }

    uint64_t x = a.digit_in_abs_format(i);
    uint64_t y = b.digit_in_abs_format(i);
    if (x < y)
        x += carry;
    else
        y += carry;
    tmp = x + y;
    result[i] = static_cast<uint32_t>(tmp);

    bool ans_sign = a.sign;
    return big_integer(result, ans_sign);
}

big_integer& big_integer::operator+=(big_integer const &rhs)
{
    return *this = *this + rhs;
}

big_integer operator-(big_integer a, big_integer const &b)
{
    if (!a.sign && b.sign)
        return a + abs(b);
    if (a.sign && !b.sign)
        return -(abs(a) + b);

    if (a.sign && b.sign)
        return (abs(b) - abs(a));

    if (a < b)
    {
        //std::cerr << a << std::endl << b << std::endl << b - a << std::endl;
        return -(b - a);
    }

    size_t max_len = std::max(a.size(), b.size());
    optimized_vector result(max_len + 1);

    size_t i = 0;
    uint64_t tmp = 0, carry = 0;
    for (; i < max_len; ++i)
    {
        uint64_t x = a.digit_in_abs_format(i);
        uint64_t y = b.digit_in_abs_format(i);
        tmp = x - y - carry;
        result[i] = static_cast<uint32_t>(tmp);
        carry = (tmp >> big_integer::LOG_BASE > 0 ? 1 : 0);
    }

    uint64_t x = a.digit_in_abs_format(i);
    uint64_t y = b.digit_in_abs_format(i);
    tmp = x - y - carry;
    result[i] = static_cast<uint32_t>(tmp);

    bool ans_sign = a.sign;
    return big_integer(result, ans_sign);
}

big_integer& big_integer::operator-=(big_integer const &rhs)
{
    return *this = *this - rhs;
}

big_integer operator*(big_integer a, big_integer const &b)
{
    if (a == 0 || b == 0)
        return 0;

    size_t a_len = a.size(), b_len = b.size();
    optimized_vector tmp(a_len + b_len + 1);
    optimized_vector ans(a_len + b_len + 1, 0);

    for (size_t i = 0; i < b_len; i++)
    {
        for (size_t j = 0; j < tmp.size(); j++)
            tmp[j] = 0;

        uint64_t mul = 0, carry = 0;
        for (size_t j = 0; j < a_len + 1; j++)
        {
            mul = static_cast<uint64_t>(a.digit_in_abs_format(j)) * b.number[i] + carry;
            carry = mul >> big_integer::LOG_BASE;
            tmp[j] = static_cast<uint32_t>(mul);
        }

        carry = 0;
        uint64_t sum = 0;
        for (size_t j = i; j < ans.size(); j++)
        {
            sum = static_cast<uint64_t>(ans[j]) + tmp[j - i] + carry;
            ans[j] = static_cast<uint32_t>(sum);
            carry = sum >> big_integer::LOG_BASE;
        }
    }

    return big_integer(ans, a.sign ^ b.sign);
}

big_integer& big_integer::operator*=(big_integer const &rhs)
{
    return *this = *this * rhs;
}

std::pair<big_integer, uint32_t> big_integer::divide_by_short(uint32_t x, bool sign)
{
    uint32_t carry = 0;
    big_integer result = *this;

    for (size_t i = this->size() - 1; i > 0; i--)
    {
        uint64_t cur = result.number[i] + carry * big_integer::BASE;
        result.number[i] = static_cast<uint32_t>(cur / x);
        carry = static_cast<uint32_t>(cur % x);
    }
    uint64_t cur = result.number[0] + carry * big_integer::BASE;
    result.number[0] = static_cast<uint32_t>(cur / x);
    carry = static_cast<uint32_t>(cur % x);
    result.sign = this->sign ^ sign;

    return {result, carry};
}

big_integer operator/(big_integer a, big_integer const &b) {
    big_integer aa(abs(a));
    big_integer bb(abs(b));

    if (aa < bb)
        return 0;
    if (aa == bb)
        return 1;
    if (bb.size() == 1)
        return a.divide_by_short(b.number[0], b.sign).first;

    auto f = static_cast<uint32_t>(big_integer::BASE / static_cast<uint64_t>(bb.number.back() + 1));
    aa *= big_integer(f);
    bb *= big_integer(f);

    size_t n = aa.size();
    size_t m = bb.size();
    optimized_vector ans(n - m + 1);
    big_integer reminder(0);

    for (size_t i = n - 1; i > n - m; i--)
    {
        reminder <<= big_integer::LOG_BASE;
        reminder.number[0] = aa.number[i];
    }

    uint64_t div_high_digit = bb.number.back();

    for (size_t i = n - m; i > 0; i--)
    {
        //Sstd::cerr << i << std::endl;
        reminder <<= big_integer::LOG_BASE;
        reminder.number[0] = aa.number[i];

        uint64_t rem_high_digits = reminder.number.back();
        if (reminder.size() > m)
        {
            rem_high_digits *= big_integer::BASE;
            rem_high_digits += reminder.number[reminder.size() - 2];
        }

        uint64_t quot_suggest = std::min(rem_high_digits / div_high_digit, big_integer::BASE - 1);

        big_integer bb_suggest = bb * big_integer(static_cast<uint32_t>(quot_suggest));

        while (reminder < bb_suggest)
        {
            quot_suggest--;
            bb_suggest -= bb;
        }

        ans[i] = static_cast<uint32_t>(quot_suggest);
        reminder -= bb_suggest;
    }

    //std::cerr << 0 << std::endl;
    reminder <<= big_integer::LOG_BASE;
    reminder.number[0] = aa.number[0];

    uint64_t rem_high_digits = reminder.number.back();
    if (reminder.size() > m)
    {
        rem_high_digits *= big_integer::BASE;
        rem_high_digits += reminder.number[reminder.size() - 2];
    }

    uint64_t quot_suggest = std::min(rem_high_digits / div_high_digit, big_integer::BASE - 1);

    big_integer bb_suggest = bb * big_integer(static_cast<uint32_t>(quot_suggest));

    while (reminder < bb_suggest)
    {
        quot_suggest--;
        bb_suggest -= bb;
    }

    ans[0] = static_cast<uint32_t>(quot_suggest);
    reminder -= bb_suggest;

    return big_integer(ans, a.sign ^ b.sign);
}

big_integer operator%(big_integer a, big_integer const &b)
{
    return a - b * (a / b);
}

big_integer& big_integer::operator/=(big_integer const &rhs)
{
    return *this = *this / rhs;
}


big_integer& big_integer::operator%=(big_integer const &rhs)
{
    return *this = *this % rhs;
}


big_integer& big_integer::operator++()
{
    //prefix
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int)
{
    // postfix
    big_integer tmp(*this);
    *this += 1;
    return tmp;
}

big_integer& big_integer::operator--()
{
    //prefix
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int)
{
    // postfix
    big_integer tmp(*this);
    *this -= 1;
    return tmp;
}

bool operator==(big_integer const &a, big_integer const &b)
{
    if (a.first_non_zero_index == SIZE_MAX &&
            b.first_non_zero_index == SIZE_MAX)
        return true;

    if ((a.sign ^ b.sign) || a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); i++)
        if (a.number[i] != b.number[i])
            return false;
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b)
{
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b)
{
    if (a.sign && !b.sign)
        return true;
    if (!a.sign && b.sign)
        return false;

    if (a.size() < b.size())
        return !a.sign;
    if (a.size() > b.size())
        return a.sign;

    for (size_t i = a.size() - 1; i > 0; i--)
    {
        if (a.number[i] > b.number[i])
            return a.sign;
        if (a.number[i] < b.number[i])
            return !a.sign;
    }

    if (a.number[0] > b.number[0])
        return a.sign;
    if (a.number[0] < b.number[0])
        return !a.sign;
    return false;
}

bool operator<=(big_integer const &a, big_integer const &b)
{
    return (a < b) || (a == b);
}

bool operator>(big_integer const &a, big_integer const &b)
{
    return b < a;
}

bool operator>=(big_integer const &a, big_integer const &b)
{
    return b <= a;
}

std::string to_string(big_integer const& a)
{
    if (a == 0)
        return "0";

    bool sign = a.sign;
    big_integer x = a;
    x.sign = false;
    std::string result;

    while (x > 0)
    {
        std::pair <big_integer, uint32_t> tmp = x.divide_by_short(10);
        x = tmp.first;
        result += static_cast<char>(tmp.second) + '0';
    }
    if (sign)
        result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    s << to_string(a);
    return s;
}


big_integer operator&(big_integer a, big_integer const& b)
{
    optimized_vector result;
    for (size_t i = 0; i <= std::max(a.size(), b.size()); ++i)
    {
        result.push_back(a.digit_in_twos_complement(i) &
                         b.digit_in_twos_complement(i));
    }
    bool ans_sign = false;
    if (result.back() > 0)
        ans_sign = true;
    return big_integer(result, ans_sign, true);
}


big_integer operator^(big_integer a, big_integer const& b)
{
    optimized_vector result;
    for (size_t i = 0; i <= std::max(a.size(), b.size()); ++i)
    {
        result.push_back(a.digit_in_twos_complement(i) ^
                         b.digit_in_twos_complement(i));
    }
    bool ans_sign = false;
    if (result.back() > 0)
        ans_sign = true;
    return big_integer(result, ans_sign, true);
}


big_integer operator|(big_integer a, big_integer const& b)
{
    optimized_vector result;
    for (size_t i = 0; i <= std::max(a.size(), b.size()); ++i)
    {
        result.push_back(a.digit_in_twos_complement(i) |
                         b.digit_in_twos_complement(i));
    }
    bool ans_sign = false;
    if (result.back() > 0)
        ans_sign = true;
    return big_integer(result, ans_sign, true);
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
    return *this = *this & rhs;
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
    return *this = *this | rhs;

}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    return *this = *this ^ rhs;
}


big_integer big_integer::operator~() const
{
    optimized_vector result;
    for (size_t i = 0; i < this->size(); ++i)
    {
        result.push_back(~(this->digit_in_twos_complement(i)));
    }
    bool ans_sign = false;
    if (!(this->number.back() & (1u << (LOG_BASE - 1))))
        ans_sign = true;
    return big_integer(result, ans_sign, true);
}


void emplace_shl(optimized_vector const &src, int b, optimized_vector &dest)
{
    //dest = src;
    dest.resize(src.size());
    std::copy(src.begin(), src.end(), dest.begin());
    std::reverse(dest.begin(), dest.end());
    while (static_cast<uint32_t>(b) >= big_integer::LOG_BASE)
    {
        dest.push_back(0);
        b -= big_integer::LOG_BASE;
    }
    std::reverse(dest.begin(), dest.end());
    if (b > 0)
    {
        // b in [1..31]
        dest.push_back(0);
        for (size_t bit = big_integer::LOG_BASE * dest.size() - 1; bit > 0; bit--)
        {
            size_t next_bit;

            dest[bit / big_integer::LOG_BASE] &= ~(1u << (bit % big_integer::LOG_BASE));

            if (bit >= static_cast<size_t>(b))
            {
                next_bit = bit - static_cast<size_t>(b);

                if (dest[next_bit / big_integer::LOG_BASE] & (1u << (next_bit % big_integer::LOG_BASE)))
                {
                    dest[bit / big_integer::LOG_BASE] |= (1u << (bit % big_integer::LOG_BASE));
                }
            }
        }

        dest[0] &= ~(1u);
    }
}

big_integer operator<<(big_integer a, int b)
{
    optimized_vector tmp;
    emplace_shl(a.number, b, tmp);
    return big_integer(tmp, a.sign);
}

big_integer& big_integer::operator<<=(int rhs)
{
    emplace_shl(this->number, rhs, this->number);
    this->normalize();
    return *this;
}

void emplace_shr(optimized_vector const &src, int b, optimized_vector &dest)
{
    dest.resize(src.size() - b / big_integer::LOG_BASE);
    b %= big_integer::LOG_BASE;

    if (b > 0)
    {
        dest.push_back(0);
        for (size_t i = 0; i < dest.size() - 1; i++)
        {
            uint64_t tmp = (static_cast<uint64_t>((i + 1 < src.size() ? src[i + 1] : 0)) << (big_integer::LOG_BASE))
                           ^ static_cast<uint64_t>(src[i]);
            tmp >>= b;
            dest[i] = static_cast<uint32_t>(tmp & UINT32_MAX);
            dest[i + 1] &= ~((1u << b) - 1);
            dest[i + 1] |= static_cast<uint32_t>(tmp >> big_integer::LOG_BASE);
        }
    }
}

big_integer operator>>(big_integer a, int b)
{
    optimized_vector res;
    emplace_shr(a.number, b, res);
    big_integer tmp(res, a.sign);
    if (a.sign)
        tmp -= 1;
    tmp.normalize();
    return tmp;
}

big_integer& big_integer::operator>>=(int rhs)
{
    emplace_shr(this->number, rhs, this->number);
    if (this->sign)
        *this -= 1;
    this->normalize();
    return *this;
}
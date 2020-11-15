#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <random>

/**
 * Unsigned huge integer!
 */
class UHugeInt {
public:
    UHugeInt(uint64_t value = 0);

    explicit UHugeInt(const std::string &value);

    UHugeInt &operator+=(const UHugeInt &other);

    UHugeInt &operator+=(uint64_t other);

    UHugeInt &operator-=(const UHugeInt &other);

    UHugeInt &operator-=(uint64_t other);

    UHugeInt &operator*=(const UHugeInt &other);

    UHugeInt &operator*=(uint64_t other);

    UHugeInt &operator/=(const UHugeInt &other);

    UHugeInt &operator/=(uint64_t other);

    UHugeInt &operator%=(const UHugeInt &other);

    UHugeInt &operator%=(uint64_t other);

    UHugeInt &operator>>=(uint64_t bits);

    UHugeInt &operator<<=(uint64_t bits);

    UHugeInt operator+(UHugeInt other) const;

    UHugeInt operator+(uint64_t other) const;

    UHugeInt operator-(UHugeInt other) const;

    UHugeInt operator-(uint64_t other) const;

    UHugeInt operator*(UHugeInt other) const;

    UHugeInt operator*(uint64_t other) const;

    UHugeInt operator/(UHugeInt other) const;

    UHugeInt operator/(uint64_t other) const;

    UHugeInt operator%(UHugeInt other) const;

    UHugeInt operator%(uint64_t other) const;

    UHugeInt operator<<(uint64_t other) const;

    UHugeInt operator>>(uint64_t other) const;

    bool operator<(const UHugeInt &other) const;

    bool operator>(const UHugeInt &other) const;

    bool operator<=(const UHugeInt &other) const;

    bool operator>=(const UHugeInt &other) const;

    bool operator==(const UHugeInt &other) const;

    bool operator!=(const UHugeInt &other) const;

    bool operator==(uint64_t value) const;

    bool operator!=(uint64_t value) const;

    bool IsZero() const;

    bool IsOdd() const;

    uint64_t BitSize() const;

    uint64_t ToUint64() const;

    static UHugeInt PowMod(UHugeInt a, UHugeInt b, const UHugeInt & mod);

    static UHugeInt Rand(const UHugeInt & max, std::mt19937_64 & rng);

    static UHugeInt Rand(const UHugeInt & min, const UHugeInt & max, std::mt19937_64 & rng);

    static UHugeInt FromBytes(const std::vector<uint8_t> & bytes);

    friend std::ostream & operator<<(std::ostream & out, const UHugeInt & val);

    std::vector<uint8_t> ToBytes() const;

protected:
    void Trunc();

    UHugeInt(std::vector<uint64_t>::const_iterator begin, std::vector<uint64_t>::const_iterator end);

    static std::pair<UHugeInt, UHugeInt> DivMod(UHugeInt a, UHugeInt b);

    void AppendDigitBack(uint64_t digit);

    uint64_t GetTopTwoDigits() const;

    uint64_t GetTopDigit() const;

    std::vector<uint64_t> digits;
};

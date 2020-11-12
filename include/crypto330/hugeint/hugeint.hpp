#pragma once

#include <cstdint>
#include <string>
#include <vector>

class UHugeInt {
public:
    explicit UHugeInt(uint64_t value = 0);

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

    uint64_t ToUint64() const;

    friend std::pair<UHugeInt, UHugeInt> DivMod(UHugeInt a, UHugeInt b);

    friend UHugeInt PowMod(UHugeInt a, UHugeInt b, const UHugeInt & mod);

protected:
    void Trunc();

    UHugeInt(std::vector<uint64_t>::const_iterator begin, std::vector<uint64_t>::const_iterator end);

    void AppendDigitBack(uint64_t digit);

    uint64_t GetTopTwoDigits() const;

    uint64_t GetTopDigit() const;

    std::vector<uint64_t> digits;
};

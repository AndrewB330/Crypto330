#include <cassert>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "crypto330/hugeint/hugeint.hpp"

const uint64_t DIGIT_SIZE = 32; // in bits
const uint64_t BASE = 1ull << DIGIT_SIZE;
const uint64_t MAX_DIGIT = BASE - 1;
// x%BASE = x&MAX_DIGIT
// x*BASE = x<<DIGIT_SIZE

static_assert(DIGIT_SIZE * 2 <= sizeof(uint64_t) * 8); // overflow check
static_assert(DIGIT_SIZE % 8 == 0); // should be byte aligned

UHugeInt::UHugeInt(uint64_t value) {
    digits.push_back(value & MAX_DIGIT);
    while (value >>= DIGIT_SIZE) {
        digits.push_back(value & MAX_DIGIT);
    }
}

UHugeInt::UHugeInt(const std::string &value) {
    digits.push_back(0);
    for (char c : value) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("[UHugeInt] Can't parse unsigned decimal integer from string");
        }
        *this *= 10;
        *this += (c - '0');
    }
    Trunc();
}

UHugeInt &UHugeInt::operator+=(const UHugeInt &other) {
    if (digits.size() < other.digits.size()) {
        digits.reserve(other.digits.size() + 1);
        while (digits.size() < other.digits.size() + 1) {
            digits.push_back(0);
        }
    } else {
        digits.push_back(0);
    }
    for (size_t i = 0; i + 1 < digits.size(); i++) {
        digits[i] += (i < other.digits.size() ? other.digits[i] : 0);
        digits[i + 1] += digits[i] >> DIGIT_SIZE;
        digits[i] &= MAX_DIGIT;
    }
    if (digits.back() == 0) {
        digits.pop_back();
    }
    return *this;
}

UHugeInt &UHugeInt::operator+=(uint64_t other) {
    if (other > MAX_DIGIT) {
        return *this += UHugeInt(other);
    }
    digits[0] += other;
    for (size_t i = 0; i + 1 < digits.size() && digits[i] > MAX_DIGIT; i++) {
        digits[i + 1] += digits[i] >> DIGIT_SIZE;
        digits[i] &= MAX_DIGIT;
    }
    if (digits.back() > MAX_DIGIT) {
        digits.push_back(digits.back() >> DIGIT_SIZE);
        digits[digits.size() - 2] &= MAX_DIGIT;
    }
    return *this;
}

UHugeInt &UHugeInt::operator-=(const UHugeInt &other) {
    if (other.digits.size() > digits.size()) {
        *this = UHugeInt(0);
        return *this;
    }
    for (size_t i = 0; i < digits.size(); i++) {
        if (i < other.digits.size()) {
            if (other.digits[i] > digits[i] || (~digits[i] == 0)) {
                if (i + 1 >= digits.size()) {
                    *this = UHugeInt(0);
                    return *this;
                }
                digits[i + 1]--;
                digits[i] += BASE;
            }
            digits[i] -= other.digits[i];
        }
    }

    Trunc();

    return *this;
}

UHugeInt &UHugeInt::operator-=(uint64_t other) {
    if (other > MAX_DIGIT) {
        return *this -= UHugeInt(other);
    }
    if (digits.size() == 1 && other > digits[0]) {
        *this = UHugeInt(0);
        return *this;
    }

    if (digits[0] < other) {
        digits[1]--;
        digits[0] += BASE;
    }

    for (size_t i = 1; i + 1 < digits.size(); i++) {
        if (~digits[i] == 0) {
            digits[i + 1]--;
            digits[i] += BASE;
        }
    }
    digits[0] -= other;

    Trunc();

    return *this;
}

UHugeInt &UHugeInt::operator*=(const UHugeInt &other) {
    std::vector<uint64_t> res(digits.size() + other.digits.size() + 1);
    for (size_t i = 0; i < digits.size(); i++) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; j++) {
            res[i + j] += digits[i] * (j < other.digits.size() ? other.digits[j] : 0) + carry;
            carry = res[i + j] >> DIGIT_SIZE;
            res[i + j] &= MAX_DIGIT;
        }
    }

    digits = std::move(res);

    Trunc();

    return *this;
}

UHugeInt &UHugeInt::operator*=(uint64_t other) {
    if (other > MAX_DIGIT) {
        return *this *= UHugeInt(other);
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < digits.size() || carry; ++i) {
        if (i == digits.size())
            digits.push_back(0);
        digits[i] = carry + digits[i] * other;
        carry = digits[i] >> DIGIT_SIZE;
        digits[i] &= MAX_DIGIT;
    }
    return *this;
}

void UHugeInt::Trunc() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

bool UHugeInt::operator<(const UHugeInt &other) const {
    if (digits.size() != other.digits.size()) {
        return digits.size() < other.digits.size();
    }
    for (size_t i = 0; i < digits.size(); i++) {
        size_t j = digits.size() - 1 - i;
        if (digits[j] != other.digits[j]) {
            return digits[j] < other.digits[j];
        }
    }
    return false;
}

bool UHugeInt::operator>(const UHugeInt &other) const {
    return other < *this;
}

bool UHugeInt::operator==(const UHugeInt &other) const {
    if (digits.size() != other.digits.size()) {
        return false;
    }
    for (size_t i = 0; i < digits.size(); i++) {
        if (digits[i] != other.digits[i]) {
            return false;
        }
    }
    return true;
}

bool UHugeInt::operator!=(const UHugeInt &other) const {
    return !(*this == other);
}

bool UHugeInt::operator==(uint64_t value) const {
    return *this == UHugeInt(value);
}

bool UHugeInt::operator!=(uint64_t value) const {
    return *this != UHugeInt(value);
}

bool UHugeInt::IsZero() const {
    return digits.size() == 1 && digits[0] == 0;
}

UHugeInt::UHugeInt(std::vector<uint64_t>::const_iterator begin, std::vector<uint64_t>::const_iterator end) {
    digits = std::vector<uint64_t>(begin, end);
}

void UHugeInt::AppendDigitBack(uint64_t digit) {
    assert(digit <= MAX_DIGIT);
    digits.insert(digits.begin(), digit);
}

uint64_t UHugeInt::GetTopTwoDigits() const {
    return (digits.back() << DIGIT_SIZE) + digits[digits.size() - 2];
}

uint64_t UHugeInt::GetTopDigit() const {
    return digits.back();
}

UHugeInt UHugeInt::operator+(UHugeInt other) const {
    return other += *this;
}

UHugeInt UHugeInt::operator+(uint64_t other) const {
    return UHugeInt(*this) += other;
}

UHugeInt UHugeInt::operator*(UHugeInt other) const {
    return other *= *this;
}

UHugeInt UHugeInt::operator*(uint64_t other) const {
    return UHugeInt(*this) *= other;
}

bool UHugeInt::operator<=(const UHugeInt &other) const {
    return !(*this > other);
}

bool UHugeInt::operator>=(const UHugeInt &other) const {
    return !(*this < other);
}

UHugeInt &UHugeInt::operator/=(const UHugeInt &other) {
    return *this = DivMod(*this, other).first;
}

UHugeInt &UHugeInt::operator%=(const UHugeInt &other) {
    return *this = DivMod(*this, other).second;
}

UHugeInt &UHugeInt::operator%=(uint64_t other) {
    uint64_t res = 0;
    for (size_t i = 0; i < digits.size(); i++) {
        res <<= DIGIT_SIZE;
        res += digits[digits.size() - i - 1];
        res %= other;
    }
    return *this = UHugeInt(res);
}

UHugeInt &UHugeInt::operator/=(uint64_t other) {
    if (other > MAX_DIGIT) {
        return *this /= UHugeInt(other);
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < digits.size(); i++) {
        size_t j = digits.size() - i - 1;
        uint64_t cur = digits[j] + (carry << DIGIT_SIZE);
        digits[j] = cur / other;
        carry = cur % other;
    }
    Trunc();
    return *this;
}

UHugeInt UHugeInt::operator-(UHugeInt other) const {
    return UHugeInt(*this) -= other;
}

UHugeInt UHugeInt::operator-(uint64_t other) const {
    return UHugeInt(*this) -= other;
}

UHugeInt UHugeInt::operator/(UHugeInt other) const {
    return DivMod(*this, other).first;
}

UHugeInt UHugeInt::operator/(uint64_t other) const {
    return UHugeInt(*this) /= other;
}

UHugeInt UHugeInt::operator%(UHugeInt other) const {
    return DivMod(*this, other).second;
}

UHugeInt UHugeInt::operator%(uint64_t other) const {
    return UHugeInt(*this) %= other;
}

uint64_t UHugeInt::ToUint64() const {
    uint64_t res = 0;
    for (size_t i = 0; i < digits.size(); i++) {
        res = (res << DIGIT_SIZE) + digits[digits.size() - i - 1];
    }
    return res;
}

UHugeInt &UHugeInt::operator>>=(uint64_t bits) {
    uint64_t digits_offset = bits / DIGIT_SIZE;
    uint64_t offset = bits % DIGIT_SIZE;

    if (digits_offset >= digits.size()) {
        return *this = UHugeInt(0);
    }

    digits.erase(digits.begin(), digits.begin() + digits_offset);
    digits[0] >>= offset;
    for (size_t i = 0; i + 1 < digits.size(); i++) {
        digits[i] |= (digits[i + 1] << (DIGIT_SIZE - offset)) & MAX_DIGIT;
        digits[i + 1] >>= offset;
    }

    Trunc();

    return *this;
}

UHugeInt &UHugeInt::operator<<=(uint64_t bits) {
    uint64_t digits_offset = bits / DIGIT_SIZE;
    uint64_t offset = bits % DIGIT_SIZE;

    uint64_t prefix = digits.back() >> (DIGIT_SIZE - offset);

    size_t prev_size = digits.size();
    digits.reserve(digits.size() + digits_offset + 1);
    digits.resize(digits.size() + digits_offset);

    for (size_t i = 0; i + 1 < prev_size; i++) {
        size_t j = prev_size - i - 1;
        digits[j + digits_offset] = ((digits[j] << offset) | (digits[j - 1] >> (DIGIT_SIZE - offset))) & MAX_DIGIT;
    }

    digits[digits_offset] = (digits[0] << offset) & MAX_DIGIT;

    if (prefix) {
        digits.push_back(prefix);
    }

    for (size_t i = 0; i < digits_offset; i++) {
        digits[i] = 0;
    }

    Trunc();

    return *this;
}

std::pair<UHugeInt, UHugeInt> UHugeInt::DivMod(UHugeInt a, UHugeInt b) {
    if (b.IsZero()) {
        throw std::invalid_argument("[UHugeInt] Division by zero");
    }
    if (b > a) {
        return {UHugeInt(0), a};
    }
    if (b.digits.size() == 1) {
        return {a / b.digits[0], a % b.digits[0]};
    }

    uint64_t offset = 0;
    while ((b.GetTopDigit() << offset) <= (MAX_DIGIT >> 1)) {
        offset++;
    }

    a <<= offset;
    b <<= offset;

    UHugeInt current = UHugeInt(a.digits.end() - b.digits.size() + 1, a.digits.end());
    auto pointer = a.digits.end() - b.digits.size() + 1;
    UHugeInt res;
    do {
        pointer--;
        current.AppendDigitBack(*pointer);
        current.Trunc();

        uint64_t digit = 0;
        if (current.digits.size() == b.digits.size()) {
            digit = current.GetTopTwoDigits() / b.GetTopTwoDigits();
        } else if (current.digits.size() > b.digits.size()) {
            digit = current.GetTopTwoDigits() / b.GetTopDigit();
        }
        if (digit) {
            if (current < b * digit) {
                digit--;
            }
            if (current < b * digit) {
                digit--;
            }
            assert(digit <= MAX_DIGIT);
            assert(current >= b * digit);
            current -= b * digit;
        }
        res.AppendDigitBack(digit);
    } while (pointer != a.digits.begin());
    res.Trunc();
    return {res, current >> offset};
}

UHugeInt UHugeInt::operator<<(uint64_t other) const {
    return UHugeInt(*this) <<= other;
}

UHugeInt UHugeInt::operator>>(uint64_t other) const {
    return UHugeInt(*this) >>= other;
}

UHugeInt UHugeInt::PowMod(UHugeInt a, UHugeInt b, const UHugeInt &mod) {
    UHugeInt res = UHugeInt(1);
    while (!b.IsZero()) {
        if (b.IsOdd()) {
            res *= a;
            res %= mod;
        }
        a *= a;
        a %= mod;
        b >>= 1;
    }
    return res;
}

bool UHugeInt::IsOdd() const {
    return digits[0] & 1;
}

UHugeInt UHugeInt::Rand(const UHugeInt &max, std::mt19937_64 &rng) {
    UHugeInt r;
    r.digits.clear();
    for (size_t i = 0; i < max.digits.size() + 8; i++) {
        r.digits.push_back(rng() % (BASE - 1) + 1);
    }
    return r % (max + 1);
}

UHugeInt UHugeInt::Rand(const UHugeInt &min, const UHugeInt &max, std::mt19937_64 &rng) {
    return Rand(max - min, rng) + min;
}

uint64_t UHugeInt::BitSize() const {
    uint64_t res = (digits.size() - 1) * DIGIT_SIZE;
    uint64_t x = GetTopDigit();
    do {
        res++;
        x >>= 1;
    } while (x);
    return res;
}

std::ostream &operator<<(std::ostream &out, const UHugeInt &val) {
    std::string res;
    UHugeInt x = val;
    do {
        res += '0' + (x % 10).ToUint64();
        x /= 10;
    } while (!x.IsZero());
    std::reverse(res.begin(), res.end());
    return out << res;
}

std::vector<uint8_t> UHugeInt::ToBytes() const {
    std::vector<uint8_t> bytes;
    for (uint64_t digit : digits) {
        bytes.insert(bytes.end(), (uint8_t *) (&digit), (uint8_t *) (&digit) + DIGIT_SIZE / 8);
    }
    return bytes;
}

UHugeInt UHugeInt::FromBytes(const std::vector<uint8_t> &bytes) {
    UHugeInt number;
    number.digits.clear();
    for (uint64_t offset = 0; offset < bytes.size(); offset += DIGIT_SIZE / 8) {
        uint64_t digit = 0;
        std::copy(bytes.begin() + offset, bytes.begin() + std::min<size_t>(offset + DIGIT_SIZE / 8, bytes.size()),
                  (uint8_t *) &digit);
        assert(digit <= MAX_DIGIT);
        number.digits.push_back(digit);
    }
    number.Trunc();
    return number;
}

UHugeInt UHugeInt::FromHex(const std::string &hex) {
    UHugeInt res;
    for(auto c : hex) {
        res <<= 4;
        res += (c >= '0' && c <= '9' ? c - '0' : (c < 'a' ? c - 'A' : c - 'a') + 10);
    }
    return res;
}

HugePolyF2::HugePolyF2(const UHugeInt &poly) : poly(poly) {}

HugePolyF2::HugePolyF2(const std::vector<uint64_t> &powers) {
    for (uint64_t p : powers) {
        poly += (UHugeInt(1) << p);
    }
}

HugePolyF2 &HugePolyF2::operator+=(const HugePolyF2 &other) {
    if (poly.digits.size() < other.poly.digits.size()) {
        while (poly.digits.size() < other.poly.digits.size()) {
            poly.digits.push_back(0);
        }
    }

    for (size_t i = 0; i < poly.digits.size(); i++) {
        poly.digits[i] ^= (i < other.poly.digits.size() ? other.poly.digits[i] : 0);
    }
    poly.Trunc();
    return *this;
}

uint64_t PolyMul(uint64_t a, uint64_t b) {
    uint64_t res = 0;
    while (b) {
        if (b & 1)
            res ^= a;
        a = a << 1;
        b >>= 1;
    }
    return res;
}

HugePolyF2 &HugePolyF2::operator*=(const HugePolyF2 &other) {
    std::vector<uint64_t> res(poly.digits.size() + other.poly.digits.size());
    for (size_t i = 0; i < poly.digits.size(); i++) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.poly.digits.size() || carry; j++) {
            res[i + j] ^= PolyMul(poly.digits[i], (j < other.poly.digits.size() ? other.poly.digits[j] : 0)) ^ carry;
            carry = res[i + j] >> DIGIT_SIZE;
            res[i + j] &= MAX_DIGIT;
        }
    }

    poly.digits = std::move(res);
    poly.Trunc();

    return *this;
}

HugePolyF2 &HugePolyF2::operator/=(const HugePolyF2 &other) {
    return *this = DivMod(*this, other).first;
}

HugePolyF2 &HugePolyF2::operator%=(const HugePolyF2 &other) {
    return *this = DivMod(*this, other).second;
}

std::pair<HugePolyF2, HugePolyF2> HugePolyF2::DivMod(HugePolyF2 a, HugePolyF2 b) {
    if (a.poly.BitSize() < b.poly.BitSize()) {
        return {HugePolyF2(0), a};
    }
    uint64_t c = (a.poly.BitSize() - b.poly.BitSize());
    HugePolyF2 res;
    HugePolyF2 tmp = a;
    for (uint64_t i = 0; i <= c; i++) {
        uint64_t j = c - i;
        int tt = tmp.poly.BitSize();
        int tt1 = (b << j).poly.BitSize();
        if (tmp.poly.BitSize() == a.poly.BitSize() - i) {
            tmp = tmp + (b << j);
            int ttt = tmp.poly.BitSize();
            res = res + (HugePolyF2(1) << j);
        }
    }
    return {res, tmp};
}

HugePolyF2 &HugePolyF2::operator>>=(uint64_t bits) {
    poly >>= bits;
    return *this;
}

HugePolyF2 &HugePolyF2::operator<<=(uint64_t bits) {
    poly <<= bits;
    return *this;
}

HugePolyF2 HugePolyF2::operator+(HugePolyF2 other) const {
    return other += *this;
}

HugePolyF2 HugePolyF2::operator*(HugePolyF2 other) const {
    return other *= *this;
}

HugePolyF2 HugePolyF2::operator/(HugePolyF2 other) const {
    return DivMod(*this, other).first;
}

HugePolyF2 HugePolyF2::operator%(HugePolyF2 other) const {
    return DivMod(*this, other).second;
}

HugePolyF2 HugePolyF2::operator<<(uint64_t other) const {
    return HugePolyF2(*this) <<= other;
}

HugePolyF2 HugePolyF2::operator>>(uint64_t other) const {
    return HugePolyF2(*this) >>= other;
}

UHugeInt HugePolyF2::ToUHugeInt() const {
    return poly;
}

bool HugePolyF2::operator==(const HugePolyF2 &other) const {
    return poly == other.poly;
}

bool HugePolyF2::operator!=(const HugePolyF2 &other) const {
    return !(*this == other);
}

HugePolyF2 HugePolyF2::FromHex(const std::string &hex) {
    return HugePolyF2(UHugeInt::FromHex(hex));
}

uint64_t HugePolyF2::ToUint64() const {
    return poly.ToUint64();
}

#include <crypto330/utils.hpp>
#include <vector>
#include <cassert>

uint8_t mulX(uint8_t x, uint8_t poly) {
    return (x << 1u) ^ (((x >> 7u) & 1u) * poly);
}

uint8_t GF8_Mul(uint8_t a, uint8_t b, uint8_t poly) {
    uint8_t res = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (a & (1 << i)) {
            res ^= b;
        }
        b = mulX(b, poly);
    }
    return res;
}

inline uint8_t ConvertToByte(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return c - 'A' + 10;
}

inline uint8_t ConvertToByte(char a, char b) {
    return (ConvertToByte(a) << 4) + ConvertToByte(b);
}

std::vector<uint8_t> HexStringToBytes(const std::string &str) {
    assert(str.size() % 2 == 0);
    std::vector<uint8_t> data(str.size() / 2);
    for (int i = 0; i * 2 + 1 < str.size(); i++) {
        data[i] = ConvertToByte(str[i * 2], str[i * 2 + 1]);
    }
    return data;
}

std::vector<uint8_t> StringToBytes(const std::string &str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}
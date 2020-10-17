#include <crypto330/block/utils.hpp>
#include <vector>
#include <cassert>

uint8_t gf8_lookup[256][256];
uint8_t gf8_lookup_kalyna[256][256];
bool gf8_lookup_initialized = false;
bool gf8_lookup_kalyna_initialized = false;

uint8_t mulX(uint8_t x, uint8_t poly) {
    return (x << 1u) ^ (((x >> 7u) & 1u) * poly);
}

uint8_t mul(uint8_t a, uint8_t b, uint8_t poly) {
    uint8_t res = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (a & (1 << i)) {
            res ^= b;
        }
        b = mulX(b, poly);
    }
    return res;
}

void GF8_InitLookup() {
    if (gf8_lookup_initialized) {
        return;
    }
    for(uint32_t a = 0; a < 256; a++) {
        for(uint32_t b = 0; b < 256; b++) {
            gf8_lookup[a][b] = mul(a, b, 0x1bu);
        }
    }
    gf8_lookup_initialized = true;
}

void GF8_InitLookupKalyna() {
    if (gf8_lookup_kalyna_initialized) {
        return;
    }
    for(uint32_t a = 0; a < 256; a++) {
        for(uint32_t b = 0; b < 256; b++) {
            gf8_lookup_kalyna[a][b] = mul(a, b, 0x1d);
        }
    }
    gf8_lookup_kalyna_initialized = true;
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
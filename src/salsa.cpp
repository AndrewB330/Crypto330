#include <crypto330/stream/salsa.hpp>
#include <cassert>

uint32_t rot(uint32_t a, uint32_t b) {
    return (a << b) | (a >> (32 - b));
}

void qr(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    b ^= rot(a + d, 7);
    c ^= rot(b + a, 9);
    d ^= rot(c + b, 13);
    a ^= rot(d + c, 18);
}

void Salsa::ProcessData(std::vector<uint8_t> &data) const {
    uint32_t state[16];
    uint32_t key_pos[] = {1, 2, 3, 4, 11, 12, 13, 14};

    for (uint64_t byte = 0; byte < data.size(); byte++) {
        if (byte % 64 == 0) {
            state[0] = 0x61707865;
            state[5] = 0x3320646e;
            state[10] = 0x79622d32;
            state[15] = 0x6b206574;
            for (uint64_t i = 0; i < 8; i++) {
                state[key_pos[i]] = key[i];
            }
            state[6] = state[8] = byte >> 32;
            state[7] = state[9] = byte & 0xFFFFFFFF;

            for (uint32_t i = 0; i < 10; i++) {
                qr(state[0], state[4], state[8], state[12]);    // column 1
                qr(state[5], state[9], state[13], state[1]);    // column 2
                qr(state[10], state[14], state[2], state[6]);    // column 3
                qr(state[15], state[3], state[7], state[11]);    // column 4
                qr(state[0], state[1], state[2], state[3]);    // row 1
                qr(state[5], state[6], state[7], state[4]);    // row 2
                qr(state[10], state[11], state[8], state[9]);    // row 3
                qr(state[15], state[12], state[13], state[14]);    // row 4
            }
        }
        data[byte] ^= reinterpret_cast<uint8_t*>(state)[byte % 64];
    }
}

void Salsa::Encrypt(std::vector<uint8_t> &data) const {
    ProcessData(data);
}

void Salsa::Decrypt(std::vector<uint8_t> &data) const {
    ProcessData(data);
}

Salsa::Salsa(const std::string &key_str) {
    assert(key_str.size() <= 32);
    std::copy(key_str.begin(), key_str.end(), key);
}

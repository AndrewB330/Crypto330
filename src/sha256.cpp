#include <crypto330/hash/sha256.hpp>
#include <iostream>

uint32_t roots[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t rot_right(uint32_t x, uint8_t n) {
    return (x >> n) | (x << (32u - n));
}

std::vector<uint8_t> Sha256::GetHash(const std::vector<uint8_t> &data) const {
    uint64_t l = data.size() * 8; // length in bits
    uint64_t blocks_num = (data.size() + 72) / 64; // number of blocks

    uint8_t block[64];

    uint32_t hash[8] = {
            0x6a09e667,
            0xbb67ae85,
            0x3c6ef372,
            0xa54ff53a,
            0x510e527f,
            0x9b05688c,
            0x1f83d9ab,
            0x5be0cd19
    };

    for (uint64_t i = 0; i < blocks_num; i++) {
        uint64_t offset = i * 64;
        std::fill(block, block + 64, 0);
        if (i == blocks_num - 1) {
            if (offset <= data.size()) {
                std::copy(data.begin() + offset, data.end(), block);
                block[data.size() - offset] = 0x80;
            }
            for (uint8_t j = 0; j < 8; j++) {
                block[63 - j] = (l >> ((j * 8))) & 0xFF;
            }
        } else {
            std::copy(data.begin() + offset, std::min(data.begin() + offset + 64, data.end()), block);
            if (offset + 64 > data.size()) {
                block[data.size() - offset] = 0x80;
            }
        }
        ProcessBlock(block, hash);
    }

    std::vector<uint8_t> hash_bytes(32);
    for (uint8_t i = 0; i < 8; i++) {
        hash_bytes[i * 4 + 3] = (hash[i] & 0x000000FF) >> 0;
        hash_bytes[i * 4 + 2] = (hash[i] & 0x0000FF00) >> 8;
        hash_bytes[i * 4 + 1] = (hash[i] & 0x00FF0000) >> 16;
        hash_bytes[i * 4 + 0] = (hash[i] & 0xFF000000) >> 24;
    }
    return hash_bytes;
}

void Sha256::ProcessBlock(uint8_t *data, uint32_t *hash) const {
    uint32_t w[64];
    for (uint32_t word = 0; word < 16; word++) {
        w[word] = 0;
        w[word] |= data[word * 4 + 0] << 24;
        w[word] |= data[word * 4 + 1] << 16;
        w[word] |= data[word * 4 + 2] << 8;
        w[word] |= data[word * 4 + 3];
    }

    for (uint8_t i = 16; i < 64; i++) {
        uint32_t s0 = rot_right(w[i - 15], 7) ^rot_right(w[i - 15], 18) ^(w[i - 15] >> 3);
        uint32_t s1 = rot_right(w[i - 2], 17) ^rot_right(w[i - 2], 19) ^(w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = hash[0];
    uint32_t b = hash[1];
    uint32_t c = hash[2];
    uint32_t d = hash[3];
    uint32_t e = hash[4];
    uint32_t f = hash[5];
    uint32_t g = hash[6];
    uint32_t h = hash[7];

    for (uint8_t i = 0; i < 64; i++) {
        uint32_t S1 = rot_right(e, 6) ^rot_right(e, 11) ^rot_right(e, 25);
        uint32_t temp1 = h + S1 + ((e & f) ^ ((~e) & g)) + roots[i] + w[i];
        uint32_t S0 = rot_right(a, 2) ^rot_right(a, 13) ^rot_right(a, 22);
        uint32_t temp2 = S0 + ((a & b) ^(a & c) ^(b & c));

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
}

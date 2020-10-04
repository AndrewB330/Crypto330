#include <algorithm>
#include <crypto330/aes.hpp>
#include <crypto330/utils.hpp>
#include <fstream>
#include <memory.h>
#include <omp.h>
#include <cassert>

void AES::BuildKeyExpansion() {
    std::copy(key, key + key_words * 4, key_expansion[0]);
    for (int column = key_words; column < 4 * (rounds + 1); column++) {
        if (column % key_words == 0) {
            for (uint32_t row = 0; row < 4; row++) {
                key_expansion[column][row] = AES_S_BOX[key_expansion[column - 1][(row + 1u) & 3u]] ^
                                             key_expansion[column - key_words][row] ^
                                             AES_R_CON[column / key_words][row];
            }
        } else {
            for (uint32_t row = 0; row < 4; row++) {
                key_expansion[column][row] = key_expansion[column - 1][row] ^
                                             key_expansion[column - key_words][row];
            }
        }
    }
}

void AES::AddRoundKey(uint8_t *block, uint32_t round) const {
    for (uint32_t i = 0; i < 16; i++) {
        *(block + i) ^= *(key_expansion[0] + round * 16 + i);
    }
}

void AES::SubBytes(uint8_t *block) const {
    for (uint32_t i = 0; i < 16; i++) {
        *(block + i) = AES_S_BOX[*(block + i)];
    }
}

void AES::InvSubBytes(uint8_t *block) const {
    for (uint32_t i = 0; i < 16; i++) {
        *(block + i) = AES_S_BOX_INV[*(block + i)];
    }
}

void AES::ShiftRows(uint8_t *block) const {
    uint8_t copy[16];
    memcpy(copy, block, 16);
    for (uint32_t r = 1; r < 4; r++) {
        for (uint32_t c = 0; c < 4; c++) {
            block[c * 4 + r] = copy[((c + r) & 3u) * 4 + r];
        }
    }
}

void AES::InvShiftRows(uint8_t *block) const {
    uint8_t copy[16];
    memcpy(copy, block, 16);
    for (uint32_t r = 1; r < 4; r++) {
        for (uint32_t c = 0; c < 4; c++) {
            block[((c + r) & 3u) * 4 + r] = copy[c * 4 + r];
        }
    }
}

void AES::MixColumns(uint8_t *block) const {
    for (uint32_t i = 0; i < 4; i++) {
        uint8_t a = block[i * 4 + 0];
        uint8_t b = block[i * 4 + 1];
        uint8_t c = block[i * 4 + 2];
        uint8_t d = block[i * 4 + 3];

        block[i * 4 + 0] = GF8_Mul(0x02, a ^ b) ^ b ^ c ^ d;
        block[i * 4 + 1] = GF8_Mul(0x02, b ^ c) ^ a ^ c ^ d;
        block[i * 4 + 2] = GF8_Mul(0x02, c ^ d) ^ a ^ b ^ d;
        block[i * 4 + 3] = GF8_Mul(0x02, d ^ a) ^ a ^ b ^ c;
    }
}

void AES::InvMixColumns(uint8_t *block) const {
    for (uint32_t i = 0; i < 4; i++) {
        uint8_t a = block[i * 4 + 0];
        uint8_t b = block[i * 4 + 1];
        uint8_t c = block[i * 4 + 2];
        uint8_t d = block[i * 4 + 3];
        block[i * 4 + 0] = GF8_Mul(0x0e, a) ^ GF8_Mul(0x0b, b) ^ GF8_Mul(0x0d, c) ^ GF8_Mul(0x09, d);
        block[i * 4 + 1] = GF8_Mul(0x09, a) ^ GF8_Mul(0x0e, b) ^ GF8_Mul(0x0b, c) ^ GF8_Mul(0x0d, d);
        block[i * 4 + 2] = GF8_Mul(0x0d, a) ^ GF8_Mul(0x09, b) ^ GF8_Mul(0x0e, c) ^ GF8_Mul(0x0b, d);
        block[i * 4 + 3] = GF8_Mul(0x0b, a) ^ GF8_Mul(0x0d, b) ^ GF8_Mul(0x09, c) ^ GF8_Mul(0x0e, d);
    }
}

void AES::EncryptBlock(uint8_t *block) const {
    AddRoundKey(block, 0);
    for (uint32_t round = 1; round < rounds; round++) {
        SubBytes(block);
        ShiftRows(block);
        MixColumns(block);
        AddRoundKey(block, round);
    }
    SubBytes(block);
    ShiftRows(block);
    AddRoundKey(block, rounds);
}

void AES::DecryptBlock(uint8_t *block) const {
    AddRoundKey(block, rounds);
    InvShiftRows(block);
    InvSubBytes(block);
    for (uint32_t round = rounds - 1; round > 0; round--) {
        AddRoundKey(block, round);
        InvMixColumns(block);
        InvShiftRows(block);
        InvSubBytes(block);
    }
    AddRoundKey(block, 0);
}

AES::AES(AES::Type type, const std::string &key_str, bool hex) {
    GF8_InitLookup();
    switch (type) {
        case Type::AES128:
            key_words = 4;
            break;
        case Type::AES192:
            key_words = 6;
            break;
        case Type::AES256:
            key_words = 8;
            break;
    }
    block_bytes = 16;
    rounds = 6 + key_words;
    std::vector<uint8_t> key_bytes;
    if (hex) {
        assert(key_str.size() <= 32);
        key_bytes = HexStringToBytes(key_str);
    } else {
        assert(key_str.size() <= 16);
        key_bytes = StringToBytes(key_str);
    }
    std::copy(key_bytes.begin(), key_bytes.end(), key);
    BuildKeyExpansion();
}

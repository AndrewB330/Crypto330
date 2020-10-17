#include <crypto330/block/kalyna.hpp>
#include <algorithm>
#include <crypto330/block/utils.hpp>
#include <cassert>

void Kalyna::BuildKeyExpansion() {
    uint8_t k0[64]{};
    uint8_t k1[64]{};
    uint8_t kt[64]{};

    if (block_words == key_words) {
        std::copy(key, key + block_words * 8, k0);
        std::copy(key, key + block_words * 8, k1);
    } else {
        std::copy(key, key + block_words * 8, k0);
        std::copy(key + block_words * 8, key + block_words * 16, k1);
    }

    kt[0] = block_words + key_words + 1;

    AddRoundKeyExpand(kt, k0);
    EncryptRound(kt);

    XorRoundKeyExpand(kt, k1);
    EncryptRound(kt);

    AddRoundKeyExpand(kt, k0);
    EncryptRound(kt);

    uint8_t temp[64];
    for (int i = 0; i < 64; i += 2) {
        temp[i + 0] = 0x01;
        temp[i + 1] = 0x00;
    }

    uint8_t kt_round[64];
    uint8_t key_rot[64];
    std::copy(key, key + key_words * 8, key_rot);
    for (uint64_t round = 0; round <= rounds_num; round += 2) {
        CopyBlockTo(kt, kt_round);
        AddRoundKeyExpand(kt_round, temp);

        CopyBlockTo(key_rot, key_expansion[round]);

        AddRoundKeyExpand(key_expansion[round], kt_round);
        EncryptRound(key_expansion[round]);
        XorRoundKeyExpand(key_expansion[round], kt_round);
        EncryptRound(key_expansion[round]);
        AddRoundKeyExpand(key_expansion[round], kt_round);

        CopyBlockTo(key_expansion[round], key_expansion[round + 1]);
        RotateLeft(key_expansion[round + 1]);

        if (key_words != block_words) {
            round += 2;

            ShiftLeft(temp);

            CopyBlockTo(kt, kt_round);
            AddRoundKeyExpand(kt_round, temp);

            CopyBlockTo(key_rot + block_words * 8, key_expansion[round]);

            AddRoundKeyExpand(key_expansion[round], kt_round);
            EncryptRound(key_expansion[round]);
            XorRoundKeyExpand(key_expansion[round], kt_round);
            EncryptRound(key_expansion[round]);
            AddRoundKeyExpand(key_expansion[round], kt_round);

            CopyBlockTo(key_expansion[round], key_expansion[round + 1]);
            RotateLeft(key_expansion[round + 1]);
        }

        ShiftLeft(temp);
        for (int i = 0; i + 1 < key_words; i++) {
            std::swap(((uint64_t *) key_rot)[i], ((uint64_t *) key_rot)[i + 1]);
        }
    }
}

void Kalyna::AddRoundKeyExpand(uint8_t *block, const uint8_t *round_key) const {
    for (uint64_t i = 0; i < block_words; i++) {
        ((uint64_t *) block)[i] += ((uint64_t *) round_key)[i];
    }
}

void Kalyna::XorRoundKeyExpand(uint8_t *block, const uint8_t *round_key) const {
    for (uint64_t i = 0; i < block_words * 8; i++) {
        block[i] ^= round_key[i];
    }
}

void Kalyna::EncryptRound(uint8_t *block) const {
    SubBytes(block);
    ShiftRows(block);
    MixColumns(block);
}

void Kalyna::ShiftLeft(uint8_t *block) const {
    for (uint64_t i = 0; i < block_words; i++) {
        ((uint64_t *) block)[i] <<= 1;
    }
}

void Kalyna::RotateLeft(uint8_t *block) const {
    std::rotate(block, block + 2 * block_words + 3, block + 8 * block_words);
}

void Kalyna::CopyBlockTo(uint8_t *block, uint8_t *destination) const {
    std::copy(block, block + block_words * 8, destination);
}

void Kalyna::SubBytes(uint8_t *block) const {
    for (uint64_t i = 0; i < block_words; i++) {
        for (uint64_t j = 0; j < 8; j++) {
            block[i * 8 + j] = KALYNA_S_BOX[j & 3][block[i * 8 + j]];
        }
    }
}

void Kalyna::ShiftRows(uint8_t *block) const {
    uint8_t block_copy[64];
    CopyBlockTo(block, block_copy);
    for (uint64_t row = 0; row < 8; row++) {
        for (uint64_t col = 0; col < block_words; col++) {
            block[((col + row * block_words / 8) % block_words) * 8 + row] = block_copy[col * 8 + row];
        }
    }
}

void Kalyna::MixColumns(uint8_t *block) const {
    MulMatrix(block, KALYNA_MDS_MATRIX[0]);
}

void Kalyna::MulMatrix(uint8_t *block, const uint8_t *matrix) const {
    uint8_t res[64]{};
    for (uint64_t col = 0; col < block_words; col++) {
        for (uint64_t row = 0; row < 8; row++) {
            for (uint64_t i = 0; i < 8; i++) {
                res[col * 8 + row] ^= GF8_Mul_Kalyna(block[col * 8 + i], matrix[row * 8 + i]);
            }
        }
    }
    CopyBlockTo(res, block);
}

void Kalyna::EncryptBlock(uint8_t *block) const {
    AddRoundKeyExpand(block, key_expansion[0]);
    for (uint64_t round = 1; round < rounds_num; round++) {
        EncryptRound(block);
        XorRoundKeyExpand(block, key_expansion[round]);
    }
    EncryptRound(block);
    AddRoundKeyExpand(block, key_expansion[rounds_num]);
}

void Kalyna::DecryptBlock(uint8_t *block) const {
    SubRoundKey(block, rounds_num);
    for (uint64_t round = rounds_num - 1; round > 0; --round) {
        DecryptRound(block);
        XorRoundKey(block, round);
    }
    DecryptRound(block);
    SubRoundKey(block, 0);
}

void Kalyna::DecryptRound(uint8_t *block) const {
    InvMixColumns(block);
    InvShiftRows(block);
    InvSubBytes(block);
}

void Kalyna::AddRoundKey(uint8_t *block, uint64_t round) const {
    AddRoundKeyExpand(block, key_expansion[round]);
}

void Kalyna::XorRoundKey(uint8_t *block, uint64_t round) const {
    XorRoundKeyExpand(block, key_expansion[round]);
}

void Kalyna::SubRoundKey(uint8_t *block, uint64_t round) const {
    for (uint64_t i = 0; i < block_words; i++) {
        ((uint64_t *) block)[i] -= ((uint64_t *) key_expansion[round])[i];
    }
}

void Kalyna::InvSubBytes(uint8_t *block) const {
    for (uint64_t i = 0; i < block_words; i++) {
        for (uint64_t j = 0; j < 8; j++) {
            block[i * 8 + j] = KALYNA_S_BOX_INV[j & 3][block[i * 8 + j]];
        }
    }
}

void Kalyna::InvShiftRows(uint8_t *block) const {
    uint8_t block_copy[64];
    CopyBlockTo(block, block_copy);
    for (uint64_t row = 0; row < 8; row++) {
        for (uint64_t col = 0; col < block_words; col++) {
            block[col * 8 + row] = block_copy[((col + row * block_words / 8) % block_words) * 8 + row];
        }
    }
}

void Kalyna::InvMixColumns(uint8_t *block) const {
    MulMatrix(block, KALYNA_MDS_MATRIX_INV[0]);
}

Kalyna::Kalyna(Type type, const std::string &key_str, bool hex) {
    switch (type) {
        case Type::K128_128:
            block_words = 2;
            key_words = 2;
            rounds_num = 10;
            break;
        case Type::K256_128:
            block_words = 2;
            key_words = 4;
            rounds_num = 14;
            break;
        case Type::K256_256:
            block_words = 4;
            key_words = 4;
            rounds_num = 14;
            break;
        case Type::K512_256:
            block_words = 4;
            key_words = 8;
            rounds_num = 18;
            break;
        case Type::K512_512:
            block_words = 8;
            key_words = 8;
            rounds_num = 18;
            break;
    }
    block_bytes = block_words * 8;

    GF8_InitLookupKalyna();

    std::vector<uint8_t> key_bytes;
    if (hex) {
        assert(key_str.size() <= key_words * 16);
        key_bytes = HexStringToBytes(key_str);
    } else {
        assert(key_str.size() <= key_words * 8);
        key_bytes = StringToBytes(key_str);
    }
    std::copy(key_bytes.begin(), key_bytes.end(), key);

    BuildKeyExpansion();
}

#pragma once

#include <string>
#include <vector>
#include "base.hpp"

class Kalyna : public BlockEncryption {
public:

    enum class Type {
        K128_128,
        K256_128,
        K256_256,
        K512_256,
        K512_512,
    };

    Kalyna(Type type, const std::string &key_str, bool hex = false);

private:
    void BuildKeyExpansion();

    void EncryptBlock(uint8_t *block) const override;

    void DecryptBlock(uint8_t *block) const override;

    void AddRoundKeyExpand(uint8_t *block, const uint8_t *round_key) const;

    void XorRoundKeyExpand(uint8_t *block, const uint8_t *round_key) const;

    void AddRoundKey(uint8_t *block, uint64_t round) const;

    void XorRoundKey(uint8_t *block, uint64_t round) const;

    void SubRoundKey(uint8_t *block, uint64_t round) const;

    void EncryptRound(uint8_t *block) const;

    void ShiftLeft(uint8_t *block) const;

    void RotateLeft(uint8_t *block) const;

    void CopyBlockTo(uint8_t *block, uint8_t *destination) const;

    void SubBytes(uint8_t *block) const;

    void ShiftRows(uint8_t *block) const;

    void MixColumns(uint8_t *block) const;

    void DecryptRound(uint8_t *block) const;

    void InvSubBytes(uint8_t *block) const;

    void InvShiftRows(uint8_t *block) const;

    void InvMixColumns(uint8_t *block) const;


    void MulMatrix(uint8_t *block, const uint8_t *matrix) const;

    uint64_t rounds_num;
    uint64_t key_words;
    uint64_t block_words;

    uint8_t key[64]{};
    uint8_t key_expansion[20][64]{};
};

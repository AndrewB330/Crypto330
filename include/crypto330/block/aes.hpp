#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include "block.hpp"

class AES : public BlockEncryption {
public:
    enum class Type {
        AES128,
        AES192,
        AES256
    };

    AES(Type type, const std::string & key, bool hex = false);

private:

    void BuildKeyExpansion();

    void EncryptBlock(uint8_t *block) const override;

    void DecryptBlock(uint8_t *block) const override;

    void AddRoundKey(uint8_t *block, uint32_t round) const;

    void SubBytes(uint8_t *block) const;

    void ShiftRows(uint8_t *block) const;

    void MixColumns(uint8_t *block) const;

    void InvSubBytes(uint8_t *block) const;

    void InvShiftRows(uint8_t *block) const;

    void InvMixColumns(uint8_t *block) const;

    uint32_t key_words;
    uint32_t rounds;

    uint8_t key[32]{};
    uint8_t key_expansion[4 * (14 + 1)][4]{};
};

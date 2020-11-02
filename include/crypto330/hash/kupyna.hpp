#pragma once

#include "hash.hpp"

class Kupyna : public Hash {
public:
    enum class Size {
        Kupyna256,
        Kupyna512
    };

    explicit Kupyna(Size size = Size::Kupyna256);

    std::vector<uint8_t> GetHash(const std::vector<uint8_t> &data) const override;

private:

    void AddRoundConstant(uint8_t* block, uint64_t round, bool p_or_q) const;
    void SubBytes(uint8_t* block) const;
    void ShiftBytes(uint8_t* block) const;
    void MixColumns(uint8_t* block) const;

    void ProcessBlock(uint8_t* block, bool p_or_q) const;

    Size size;

    uint64_t block_size;
    uint64_t hash_size;
    uint64_t rounds;

    const uint64_t rows = 8;
    uint64_t columns;
};

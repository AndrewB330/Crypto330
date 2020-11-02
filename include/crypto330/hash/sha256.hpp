#pragma once

#include "hash.hpp"


class Sha256 : public Hash {
public:
    Sha256() = default;

    std::vector<uint8_t> GetHash(const std::vector<uint8_t> &data) const override;

private:
    void ProcessBlock(uint8_t *data, uint32_t hash[8]) const;
};

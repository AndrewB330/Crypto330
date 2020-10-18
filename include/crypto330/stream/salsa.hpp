#pragma once

#include "crypto330/block/block.hpp"

class Salsa {
public:
    Salsa(const std::string & key);

    void Encrypt(std::vector<uint8_t> & data) const;

    void Decrypt(std::vector<uint8_t> & data) const;

private:

    void ProcessData(std::vector<uint8_t> & data) const;

    uint32_t key[8];
};
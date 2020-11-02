#pragma once

#include <cstdint>
#include <vector>

class Hash {
public:
    Hash() = default;

    virtual std::vector<uint8_t> GetHash(const std::vector<uint8_t>& data) const = 0;
private:

};
#pragma once

#include <cstdint>
#include <string>
#include <vector>

class BlockEncryption {
public:

    void Decrypt(std::vector<uint8_t> &data) const;

    void Encrypt(std::vector<uint8_t> &data) const;

    virtual void EncryptBlock(uint8_t *block) const = 0;

    virtual void DecryptBlock(uint8_t *block) const = 0;

    uint64_t GetBlockBytes() const;

protected:
    void ProcessData(std::vector<uint8_t> &data, bool encryption) const;

    void ProcessFile(const std::string &source, const std::string &destination, bool encryption) const;

    uint64_t block_bytes = 0;
};
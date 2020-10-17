#pragma once
#include <string>
#include <vector>

class RC4 {
public:
    explicit RC4(const std::string & key);

    void Decrypt(std::vector<uint8_t> &data) const;

    void Encrypt(std::vector<uint8_t> &data) const;

private:
    void ProcessData(std::vector<uint8_t> &data) const;
    std::string key;
};
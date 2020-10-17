#include <crypto330/stream/rc4.hpp>
#include <cassert>

void RC4::Encrypt(std::vector<uint8_t> &data) const {
    ProcessData(data);
}

void RC4::Decrypt(std::vector<uint8_t> &data) const {
    ProcessData(data);
}

void RC4::ProcessData(std::vector<uint8_t> &data) const {
    uint8_t s[256];
    for(uint64_t i = 0; i < 256; i++) {
        s[i] = i;
    }
    for(uint64_t i = 0, j = 0; i < 256; i++) {
        j = (j + s[i] + key[i % key.size()]) & 0xff;
        std::swap(s[i], s[j]);
    }

    uint8_t i = 0, j = 0;
    for(uint64_t byte = 0; byte < data.size(); byte++) {
        i++;
        j+= s[i];
        std::swap(s[i], s[j]);
        data[byte] ^= s[(s[i] + s[j]) & 0xff];
    }
}

RC4::RC4(const std::string &key):key(key) {
    assert(key.size() > 4 && key.size() <= 256);
}

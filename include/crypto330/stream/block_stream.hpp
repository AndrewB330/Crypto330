#include <memory>
#include "crypto330/block/block.hpp"

class BlockStreamEncryption {
public:
    enum class Mode {
        ECB,
        CBC,
        CFB,
        OFB,
        CTR
    };

    BlockStreamEncryption(std::unique_ptr<BlockEncryption> && encryption);

    void Decrypt(std::vector<uint8_t> &data, Mode mode) const;

    void Encrypt(std::vector<uint8_t> &data, Mode mode) const;

private:
    std::unique_ptr<BlockEncryption> encryption;

    void EncryptECB(std::vector<uint8_t> &data) const;

    void DecryptECB(std::vector<uint8_t> &data) const;

    void EncryptCBC(std::vector<uint8_t> & data) const;

    void DecryptCBC(std::vector<uint8_t> & data) const;

    void EncryptCFB(std::vector<uint8_t> & data) const;

    void DecryptCFB(std::vector<uint8_t> & data) const;

    void EncryptOFB(std::vector<uint8_t> & data) const;

    void DecryptOFB(std::vector<uint8_t> & data) const;

    void EncryptCTR(std::vector<uint8_t> & data) const;

    void DecryptCTR(std::vector<uint8_t> & data) const;

    void PushUint64(std::vector<uint8_t> & data, uint64_t value) const;

    uint64_t PopUint64(std::vector<uint8_t> & data) const;

    void AlignData(std::vector<uint8_t> & data, uint64_t block_size) const;

    std::vector<uint8_t> GenerateIV(uint64_t bytes) const;
};
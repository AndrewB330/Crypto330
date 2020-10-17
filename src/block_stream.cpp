#include <crypto330/stream/block_stream.hpp>
#include <cassert>

BlockStreamEncryption::BlockStreamEncryption(std::unique_ptr<BlockEncryption> &&encryption) : encryption(std::move(encryption)) {}

void BlockStreamEncryption::Encrypt(std::vector<uint8_t> &data, BlockStreamEncryption::Mode mode) const {
    switch (mode) {
        case Mode::ECB:
            EncryptECB(data);
            break;
        case Mode::CBC:
            EncryptCBC(data);
            break;
        case Mode::CFB:
            EncryptCFB(data);
            break;
        case Mode::OFB:
            EncryptOFB(data);
            break;
        case Mode::CTR:
            EncryptCTR(data);
            break;
    }
}

void BlockStreamEncryption::Decrypt(std::vector<uint8_t> &data, BlockStreamEncryption::Mode mode) const {
    switch (mode) {
        case Mode::ECB:
            DecryptECB(data);
            break;
        case Mode::CBC:
            DecryptCBC(data);
            break;
        case Mode::CFB:
            DecryptCFB(data);
            break;
        case Mode::OFB:
            DecryptOFB(data);
            break;
        case Mode::CTR:
            DecryptCTR(data);
            break;
    }
}

void BlockStreamEncryption::EncryptECB(std::vector<uint8_t> &data) const {
    uint64_t origin_size = data.size();
    uint64_t block_size = encryption->GetBlockBytes();
    AlignData(data, block_size);
#pragma omp parallel for
    for (size_t byte = 0; byte < data.size(); byte += block_size) {
        encryption->EncryptBlock(data.data() + byte);
    }

    PushUint64(data, origin_size);
}

void BlockStreamEncryption::DecryptECB(std::vector<uint8_t> &data) const {
    uint64_t origin_size = PopUint64(data);
    uint64_t block_size = encryption->GetBlockBytes();
    AlignData(data, block_size);
#pragma omp parallel for
    for (size_t byte = 0; byte < data.size(); byte += block_size) {
        encryption->DecryptBlock(data.data() + byte);
    }

    assert(data.size() >= origin_size);
    data.resize(origin_size);
}

void BlockStreamEncryption::PushUint64(std::vector<uint8_t> &data, uint64_t value) const {
    data.resize(data.size() + sizeof(uint64_t));
    *reinterpret_cast<uint64_t*>(data.data() + data.size() - sizeof(uint64_t)) = value;
}

uint64_t BlockStreamEncryption::PopUint64(std::vector<uint8_t> &data) const {
    assert(data.size() >= sizeof(uint64_t));
    uint64_t value = *reinterpret_cast<uint64_t*>(data.data() + data.size() - sizeof(uint64_t));
    data.resize(data.size() - sizeof(uint64_t));
    return value;
}

void BlockStreamEncryption::AlignData(std::vector<uint8_t> & data, uint64_t block_size) const {
    while (data.size() % block_size != 0) {
        data.push_back(0x00);
    }
}

void BlockStreamEncryption::EncryptCBC(std::vector<uint8_t> &data) const {
    uint64_t origin_size = data.size();
    uint64_t block_size = encryption->GetBlockBytes();
    AlignData(data, block_size);
    auto iv = GenerateIV(block_size);

    data.insert(data.begin(), iv.begin(), iv.end());

    for (size_t byte = 0; byte < data.size(); byte += block_size) {
        for(uint64_t i = 0; i < block_size && byte; i++) {
            data[byte+i] ^= data[byte + i - block_size];
        }
        encryption->EncryptBlock(data.data() + byte);
    }

    PushUint64(data, origin_size);
}

void BlockStreamEncryption::DecryptCBC(std::vector<uint8_t> &data) const {
    uint64_t origin_size = PopUint64(data);
    uint64_t block_size = encryption->GetBlockBytes();
    AlignData(data, block_size);

    for (size_t byte = data.size() - block_size; byte < data.size(); byte -= block_size) {
        encryption->DecryptBlock(data.data() + byte);
        for(uint64_t i = 0; i < block_size && byte; i++) {
            data[byte+i] ^= data[byte + i - block_size];
        }
    }

    data.erase(data.begin(), data.begin() + block_size);

    assert(data.size() >= origin_size);
    data.resize(origin_size);
}

std::vector<uint8_t> BlockStreamEncryption::GenerateIV(uint64_t bytes) const {
    std::vector<uint8_t> iv(bytes);
    for(uint64_t i = 0; i < bytes; i++) {
        iv[i] = rand();
    }
    return iv;
}

void BlockStreamEncryption::EncryptOFB(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    auto iv = GenerateIV(block_size);
    auto state = iv;
    for(size_t byte = 0; byte < data.size(); byte++) {
        if (byte % block_size == 0) {
            encryption->EncryptBlock(state.data());
        }
        data[byte] ^= state[byte % block_size];
    }
    data.insert(data.end(), iv.begin(), iv.end());
}

void BlockStreamEncryption::DecryptOFB(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    auto iv = std::vector<uint8_t>(data.end()-block_size, data.end());
    data.erase(data.end()-block_size, data.end());
    auto state = iv;
    for(size_t byte = 0; byte < data.size(); byte++) {
        if (byte % block_size == 0) {
            encryption->EncryptBlock(state.data());
        }
        data[byte] ^= state[byte % block_size];
    }
}

void BlockStreamEncryption::EncryptCTR(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    uint64_t counter = 0;
    auto iv = GenerateIV(block_size);
    auto state = iv;
    assert(block_size >= sizeof(uint64_t));

    for(size_t byte = 0; byte < data.size(); byte++) {
        if (byte % block_size == 0) {
            state = iv;
            *reinterpret_cast<uint64_t*>(state.data()) ^= counter++;
            encryption->EncryptBlock(state.data());
        }
        data[byte] ^= state[byte % block_size];
    }

    data.insert(data.end(), iv.begin(), iv.end());
}

void BlockStreamEncryption::DecryptCTR(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    uint64_t counter = 0;
    auto iv = std::vector<uint8_t>(data.end()-block_size, data.end());
    data.erase(data.end()-block_size, data.end());
    auto state = iv;
    assert(block_size >= sizeof(uint64_t));

    for(size_t byte = 0; byte < data.size(); byte++) {
        if (byte % block_size == 0) {
            state = iv;
            *reinterpret_cast<uint64_t*>(state.data()) ^= counter++;
            encryption->EncryptBlock(state.data());
        }
        data[byte] ^= state[byte % block_size];
    }
}

void BlockStreamEncryption::EncryptCFB(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    assert(block_size >= 8);
    uint64_t small_block_size = block_size / 2 - 2;
    auto iv = GenerateIV(block_size);

    auto state = iv;
    uint64_t origin_size = data.size();
    AlignData(data, small_block_size);

    for(size_t byte = 0; byte < data.size(); byte+=small_block_size) {
        auto temp = state;
        encryption->EncryptBlock(temp.data());
        temp.erase(temp.begin(), temp.end() - small_block_size);
        for(uint64_t i = 0; i < small_block_size; i++) {
            data[byte + i] ^= temp[i];
        }
        state.erase(state.begin(), state.begin() + small_block_size);
        state.insert(state.end(), data.begin() + byte, data.begin() + byte + small_block_size);
    }

    PushUint64(data, origin_size);
    data.insert(data.end(), iv.begin(), iv.end());
}

void BlockStreamEncryption::DecryptCFB(std::vector<uint8_t> &data) const {
    uint64_t block_size = encryption->GetBlockBytes();
    assert(block_size >= 8);
    uint64_t small_block_size = block_size / 2 - 2;
    auto iv = std::vector<uint8_t>(data.end()-block_size, data.end());
    data.erase(data.end()-block_size, data.end());
    uint64_t origin_size = PopUint64(data);

    auto state = iv;
    AlignData(data, small_block_size);

    for(size_t byte = 0; byte < data.size(); byte+=small_block_size) {
        auto temp = state;
        auto data_copy = std::vector<uint8_t>(data.begin() + byte, data.begin() + byte + small_block_size);
        encryption->EncryptBlock(temp.data());
        temp.erase(temp.begin(), temp.end() - small_block_size);
        for(uint64_t i = 0; i < small_block_size; i++) {
            data[byte + i] ^= temp[i];
        }
        state.erase(state.begin(), state.begin() + small_block_size);
        state.insert(state.end(), data_copy.begin(), data_copy.end());
    }

    data.resize(origin_size);
}




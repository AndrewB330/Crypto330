#include <crypto330/block/block.hpp>
#include <fstream>

void BlockEncryption::EncryptFile(const std::string &source, const std::string &destination) const {
    ProcessFile(source, destination, true);
}

void BlockEncryption::DecryptFile(const std::string &source, const std::string &destination) const {
    ProcessFile(source, destination, false);
}

void BlockEncryption::Encrypt(std::vector<uint8_t> &data) const {
    ProcessData(data, true);
}

void BlockEncryption::Decrypt(std::vector<uint8_t> &data) const {
    ProcessData(data, false);
}

void BlockEncryption::ProcessFile(const std::string &source, const std::string &destination, bool encrypt) const {
    std::ifstream in(source.c_str(), std::ios_base::binary);
    std::ofstream out(destination.c_str(), std::ios_base::binary);

    in.seekg(0, std::ios::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios::beg);

    const size_t CHUNK_SIZE = 1024 * 1024 * 16;
    uint8_t chunk[CHUNK_SIZE];
    for (size_t i = 0; i < (length + CHUNK_SIZE - 1) / CHUNK_SIZE; i++) {
        uint32_t size = std::min(CHUNK_SIZE, length - i * CHUNK_SIZE);
        std::fill(chunk, chunk + CHUNK_SIZE, 0);
        in.read(reinterpret_cast<char *>(chunk), size);
#pragma omp parallel for
        for (uint32_t byte = 0; byte < size; byte += block_bytes) {
            (encrypt ? EncryptBlock(chunk + byte) : DecryptBlock(chunk + byte));
        }
        out.write(reinterpret_cast<char *>(chunk), ((size + block_bytes - 1) / block_bytes) * block_bytes);
    }

    in.close();
    out.close();
}

void BlockEncryption::ProcessData(std::vector<uint8_t> &data, bool encryption) const {
    while (data.size() % block_bytes != 0) data.push_back(0x00);
#pragma omp parallel for
    for (size_t byte = 0; byte < data.size(); byte += block_bytes) {
        (encryption ? EncryptBlock(data.data() + byte) : DecryptBlock(data.data() + byte));
    }
}

uint64_t BlockEncryption::GetBlockBytes() const {
    return block_bytes;
}

#include <iostream>
#include <crypto330/aes.hpp>
#include <fstream>
#include <chrono>
#include <crypto330/utils.hpp>
#include <crypto330/kalyna.hpp>

//UInt128 key{{0x13, 0x11, 0x95, 0xaf, 0xf3, 0xe2, 0x11, 0xaa, 0xb3, 0x41, 0x46, 0xb3, 0xf1, 0x1f, 0xfc, 0xdb}};
AES aes(AES::Type::AES128, "2b7e151628aed2a6abf7158809cf4f3c", true);
Kalyna kalyna(Kalyna::Type::K128_128, "000102030405060708090A0B0C0D0E0F", true);
Kalyna kalyna512(Kalyna::Type::K512_256, std::string() +
                                         "000102030405060708090A0B0C0D0E0F" +
                                         "101112131415161718191A1B1C1D1E1F" +
                                         "202122232425262728292A2B2C2D2E2F" +
                                         "303132333435363738393A3B3C3D3E3F", true);

void Test() {
    std::vector<uint8_t> data =
            {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    aes.Encrypt(data);
    aes.Decrypt(data);
    for (auto byte : data) std::cout << std::hex << uint32_t(byte) << " ";
    std::cout << std::endl;
}

void Test2() {
    std::vector<uint8_t> data =
            {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
             0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F};
    kalyna512.Encrypt(data);
    for (auto byte : data) std::cout << std::hex << uint32_t(byte) << " ";
    std::cout << std::endl;
    kalyna512.Decrypt(data);
    for (auto byte : data) std::cout << std::hex << uint32_t(byte) << " ";
    std::cout << std::endl;
}

int main() {
    //auto start_time = std::chrono::high_resolution_clock::now();
    //Test2();
    Test2();

    //aes.EncryptFile("Data1GB.txt", "Encrypted1GB.txt");
    //aes.DecryptFile("Encrypted1GB.txt", "Decrypted1GB.txt");


    //auto cur_time = std::chrono::high_resolution_clock::now();
    //auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(cur_time - start_time).count();
    //std::cout << "Elapsed: " << delta << "s" << std::endl;
    return 0;
}

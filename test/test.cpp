#include <gtest/gtest.h>
#include <crypto330/block/kalyna.hpp>
#include <crypto330/block/utils.hpp>
#include <crypto330/block/aes.hpp>
#include <crypto330/stream/block_stream.hpp>
#include <crypto330/stream/rc4.hpp>
#include <crypto330/stream/salsa.hpp>
#include <crypto330/hash/sha256.hpp>
#include <crypto330/hash/kupyna.hpp>
#include <fstream>

// Test data from original papers

std::string KEY128 = "000102030405060708090A0B0C0D0E0F";
std::string KEY256 = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F";
std::string KEY512 = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F"
                     "202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F";

std::string AES_KEY_128 = "2b7e151628aed2a6abf7158809cf4f3c";
std::string AES_KEY_192 = "2b7e151628aed2a6abf7158809cf4f3c";
std::string AES_KEY_256 = "2b7e151628aed2a6abf7158809cf4f3c";

TEST(Kalyna, Encryption_512_512) {
    std::vector<uint8_t> data = HexStringToBytes("404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F"
                                                 "606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F");
    std::vector<uint8_t> expected = HexStringToBytes("4A26E31B811C356AA61DD6CA0596231A67BA8354AA47F3A13E1DEEC320EB56B8"
                                                     "95D0F417175BAB662FD6F134BB15C86CCB906A26856EFEB7C5BC6472940DD9D9");

    Kalyna kalyna(Kalyna::Type::K512_512, KEY512, true);
    kalyna.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Kalyna, Encryption_512_256) {
    std::vector<uint8_t> data = HexStringToBytes("404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F");
    std::vector<uint8_t> expected = HexStringToBytes(
            "606990E9E6B7B67A4BD6D893D72268B78E02C83C3CD7E102FD2E74A8FDFE5DD9");

    Kalyna kalyna(Kalyna::Type::K512_256, KEY512, true);
    kalyna.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Kalyna, Encryption_256_256) {
    std::vector<uint8_t> data = HexStringToBytes("202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F");
    std::vector<uint8_t> expected = HexStringToBytes(
            "F66E3D570EC92135AEDAE323DCBD2A8CA03963EC206A0D5A88385C24617FD92C");

    Kalyna kalyna(Kalyna::Type::K256_256, KEY256, true);
    kalyna.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Kalyna, Encryption_256_128) {
    std::vector<uint8_t> data = HexStringToBytes("202122232425262728292A2B2C2D2E2F");
    std::vector<uint8_t> expected = HexStringToBytes("58EC3E091000158A1148F7166F334F14");

    Kalyna kalyna(Kalyna::Type::K256_128, KEY256, true);
    kalyna.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Kalyna, Encryption_128_128) {
    std::vector<uint8_t> data = HexStringToBytes("101112131415161718191A1B1C1D1E1F");
    std::vector<uint8_t> expected = HexStringToBytes("81BF1C7D779BAC20E1C9EA39B4D2AD06");

    Kalyna kalyna(Kalyna::Type::K128_128, KEY128, true);
    kalyna.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Kalyna, Decryption) {
    std::vector<uint8_t> data = StringToBytes("Hello, it's me MARIO! Maariioo! And who are you?");
    std::vector<uint8_t> expected = data;

    Kalyna kalyna(Kalyna::Type::K256_128, KEY256, true);
    kalyna.Encrypt(data);
    kalyna.Decrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(AES, Aes_128) {
    std::vector<uint8_t> data = HexStringToBytes("3243f6a8885a308d313198a2e0370734");
    std::vector<uint8_t> expected = HexStringToBytes("3925841d02dc09fbdc118597196a0b32");

    AES aes(AES::Type::AES128, AES_KEY_128, true);
    aes.Encrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(AES, Decryption) {
    std::vector<uint8_t> data = StringToBytes("Hello, it's me MARIO! Maaariioo!");
    std::vector<uint8_t> expected = data;
    AES aes(AES::Type::AES128, AES_KEY_128, true);
    aes.Encrypt(data);
    aes.Decrypt(data);

    EXPECT_EQ(data, expected);
}

TEST(Stream, ECB_AES) {
    std::vector<uint8_t> data = StringToBytes("Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, AES_KEY_128, true));

    enc.Encrypt(data, BlockStreamEncryption::Mode::ECB);
    EXPECT_NE(data, expected);
    enc.Decrypt(data, BlockStreamEncryption::Mode::ECB);
    EXPECT_EQ(data, expected);
}

TEST(Stream, CBC_AES) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, AES_KEY_128, true));

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data, BlockStreamEncryption::Mode::CBC);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data, BlockStreamEncryption::Mode::CBC);
    EXPECT_EQ(data, expected);
}

TEST(Stream, OFB_AES) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, AES_KEY_128, true));

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data, BlockStreamEncryption::Mode::OFB);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data, BlockStreamEncryption::Mode::OFB);
    EXPECT_EQ(data, expected);
}

TEST(Stream, CTR_AES) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, AES_KEY_128, true));

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data, BlockStreamEncryption::Mode::CTR);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data, BlockStreamEncryption::Mode::CTR);
    EXPECT_EQ(data, expected);
}

TEST(Stream, CFB_AES) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, AES_KEY_128, true));

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data, BlockStreamEncryption::Mode::CFB);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data, BlockStreamEncryption::Mode::CFB);
    EXPECT_EQ(data, expected);
}

TEST(Stream, RC4) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    RC4 enc("Cool RC4 Key");

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data);
    EXPECT_EQ(data, expected);
}

TEST(Stream, Salsa) {
    std::vector<uint8_t> data = StringToBytes("XX Some random data, words, and other, !5$2552ASxv b\nf");
    std::vector<uint8_t> expected = data;
    Salsa enc("Cool Salsa Key");

    EXPECT_EQ(data[0], data[1]);
    enc.Encrypt(data);
    EXPECT_NE(data, expected);
    EXPECT_NE(data[0], data[1]);
    enc.Decrypt(data);
    EXPECT_EQ(data, expected);
}

TEST(Hash, Sha256_empty) {
    Sha256 hash;
    std::vector<uint8_t> data = StringToBytes("");
    std::vector<uint8_t> expected = HexStringToBytes("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    EXPECT_EQ(hash.GetHash(data), expected);
}

TEST(Hash, Sha256_small) {
    Sha256 hash;
    std::vector<uint8_t> data = StringToBytes("shmall test %$$#");
    std::vector<uint8_t> expected = HexStringToBytes("f23f9407ba9c86b8dd1cc4adb33460816e9c316a62cda5acd69ae1485f061cd7");
    EXPECT_EQ(hash.GetHash(data), expected);
}

TEST(Benchmark, test) {
    Sha256 hash;

    uint64_t target = 5;
     for (uint64_t i = 0;; i++) {
        auto data = StringToBytes(std::to_string(i) + "a");
        auto h = hash.GetHash(data);
        uint64_t j = 0;
        uint8_t t = 0;
        while(h[j] == 0) j++, t+=2;
        if(h[j] < 16) t++;
        if (t == target) {
            std::cout << "Zero prefix found, on iteration " << i << std::endl;
            for(uint8_t byte : h) {
                printf("%02x",byte);
            }
            std::cout << std::endl;
            break;
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

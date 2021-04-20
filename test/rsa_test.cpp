#include <gtest/gtest.h>
#include <crypto330/hugeint/hugeint.hpp>
#include <crypto330/symmetric/rsa.hpp>
#include <crypto330/utils.hpp>

TEST(RSA, Basic1024_empty) {
    auto [private_key, public_key] = RSA::GenerateKeys(1024, 0);
    UHugeInt message = UHugeInt::FromBytes(StringToBytes(""));
    UHugeInt encrypted = RSA::Encrypt(message, public_key);
    UHugeInt decrypted = RSA::Decrypt(encrypted, private_key);
    EXPECT_EQ(message, decrypted);
}

TEST(RSA, Basic1024) {
    auto [private_key, public_key] = RSA::GenerateKeys(1024, 0);
    UHugeInt message = UHugeInt::FromBytes(StringToBytes("Random msg"));
    UHugeInt encrypted = RSA::Encrypt(message, public_key);
    UHugeInt decrypted = RSA::Decrypt(encrypted, private_key);
    EXPECT_EQ(message, decrypted);
}

TEST(RSA, Basic2048) {
    auto [private_key, public_key] = RSA::GenerateKeys(2048, 0);
    UHugeInt message = UHugeInt::FromBytes(StringToBytes("Random msg"));
    UHugeInt encrypted = RSA::Encrypt(message, public_key);
    UHugeInt decrypted = RSA::Decrypt(encrypted, private_key);
    EXPECT_EQ(message, decrypted);
}

TEST(RSA, OAEP_1024) {
    auto [private_key, public_key] = RSA::GenerateKeys(1024, 0);
    std::vector<uint8_t> data = StringToBytes("This is test message! RANDOM DATA DATA DATA DATA DATA DATA DATA");
    std::vector<uint8_t> encrypted = RSA::EncryptOAEP(data, public_key);
    std::vector<uint8_t> decrypted = RSA::DecryptOAEP(encrypted, private_key);
    EXPECT_EQ(data, decrypted);
}

TEST(RSA, OAEP_2048) {
    auto [private_key, public_key] = RSA::GenerateKeys(2048, 0);
    std::vector<uint8_t> data = StringToBytes("This is test message! RANDOM DATA DATA DATA DATA DATA DATA DATA");
    std::vector<uint8_t> encrypted = RSA::EncryptOAEP(data, public_key);
    std::vector<uint8_t> decrypted = RSA::DecryptOAEP(encrypted, private_key);
    EXPECT_EQ(data, decrypted);
}

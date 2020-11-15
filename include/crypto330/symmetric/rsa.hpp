#pragma once

#include <cstdint>
#include <crypto330/hugeint/hugeint.hpp>

namespace RSA {
    struct PrivateKey {
        UHugeInt p;
        UHugeInt q;
        UHugeInt d;
        UHugeInt dp;
        UHugeInt dq;
        UHugeInt q_inv;

        PrivateKey(const UHugeInt & p, const UHugeInt & q, const UHugeInt & d);
    };
    struct PublicKey {
        UHugeInt n;
        UHugeInt e;

        PublicKey(const UHugeInt & n, const UHugeInt & e);
    };

    std::pair<PrivateKey, PublicKey> GenerateKeys(uint64_t key_size, uint64_t seed);

    UHugeInt Decrypt(const UHugeInt &message, const PrivateKey &key);

    UHugeInt Encrypt(const UHugeInt &message, const PublicKey &key);

    std::vector<uint8_t> DecryptOAEP(const std::vector<uint8_t> & message, const PrivateKey & key);

    std::vector<uint8_t> EncryptOAEP(const std::vector<uint8_t> & message, const PublicKey & key);
}
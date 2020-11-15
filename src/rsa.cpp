#include <crypto330/symmetric/rsa.hpp>
#include <crypto330/hugeint/math.hpp>
#include <cassert>
#include <crypto330/hash/sha256.hpp>
#include <iostream>

const bool USE_DECRYPT_OPTIMIZATION = true;

namespace RSA {

    UHugeInt GeneratePrime(uint64_t bit_length, std::mt19937_64 &rng) {
        UHugeInt number = UHugeInt::Rand(
                UHugeInt(1) << (bit_length - 1),
                (UHugeInt(1) << bit_length) - 1,
                rng
        );
        assert(number.BitSize() == bit_length);
        if (!number.IsOdd()) number += 1;
        while (!IsProbablePrime(number)) {
            number += 2;
        }
        return number;
    }

    std::pair<PrivateKey, PublicKey> GenerateKeys(uint64_t key_size, uint64_t seed) {
        std::mt19937_64 rng;
        rng.seed(seed);
        UHugeInt p = GeneratePrime(key_size / 2, rng);
        UHugeInt q = GeneratePrime((key_size + 1) / 2, rng);
        UHugeInt n = p * q;
        UHugeInt phi = (p - 1) * (q - 1);
        UHugeInt e = 65537;
        while(GreatestCommonDivisor(e, phi) != 1) {
            e += 1;
        }
        UHugeInt d = InverseModulo(e, phi);
        assert(e * d % phi == 1);
        return {PrivateKey(p, q, d),
                PublicKey(n, e)};
    }

    UHugeInt Decrypt(const UHugeInt &message, const PrivateKey &key) {
        if (!USE_DECRYPT_OPTIMIZATION) {
            return UHugeInt::PowMod(message, key.d, key.p * key.q);
        }
        UHugeInt m1 = UHugeInt::PowMod(message, key.dp, key.p);
        UHugeInt m2 = UHugeInt::PowMod(message, key.dq, key.q);
        UHugeInt h = (key.q_inv * ((m1 + key.p) - m2 % key.p)) % key.p;
        return (m2 + h * key.q) % (key.p * key.q);
    }

    UHugeInt Encrypt(const UHugeInt &message, const PublicKey &key) {
        return UHugeInt::PowMod(message, key.e, key.n);
    }

    uint64_t k0 = 256;
    uint64_t k1 = 256;
    uint64_t padding = 8;

    std::vector<uint8_t> hash(std::vector<uint8_t> data, uint64_t target_size) {
        Sha256 sha;
        std::vector<uint8_t> res;
        while (res.size() < target_size / 8) {
            data = sha.GetHash(data);
            res.insert(res.begin(), data.begin(), data.end());
        }
        res.resize(target_size / 8);
        return res;
    }

    std::vector<uint8_t> DecryptOAEP(const std::vector<uint8_t> &message, const PrivateKey &key) {
        if (message.empty()) {
            return message;
        }
        uint64_t block_size = ((key.p * key.q).BitSize() + 7) / 8 * 8 - padding;
        auto data = message;
        while (data.size() % ((block_size + padding) / 8)) {
            data.push_back(0);
        }
        std::vector<uint8_t> res;
        for (uint64_t offset = 0; offset < data.size(); offset += (block_size + padding) / 8) {
            UHugeInt encoded_number = UHugeInt::FromBytes(
                    std::vector<uint8_t>(data.begin() + offset, data.begin() + offset + (block_size + padding) / 8)
            );
            UHugeInt decoded_number = RSA::Decrypt(encoded_number, key);
            std::vector<uint8_t> XY = decoded_number.ToBytes();
            XY.resize(block_size / 8);
            std::vector<uint8_t> X(XY.begin(), XY.begin() + (block_size - k0) / 8);
            std::vector<uint8_t> Y(XY.begin() + (block_size - k0) / 8, XY.end());
            std::vector<uint8_t> H = hash(X, k0);
            assert(H.size() == Y.size());
            for (uint64_t i = 0; i < Y.size(); i++) Y[i] ^= H[i];
            const std::vector<uint8_t> &r = Y;
            std::vector<uint8_t> G = hash(r, block_size - k0);
            assert(X.size() == G.size());
            for (uint64_t i = 0; i < X.size(); i++) X[i] ^= G[i];
            res.insert(res.end(), X.begin(), X.begin() + (block_size - k0 - k1) / 8);
        }

        uint64_t size;
        std::copy(res.end() - sizeof(uint64_t), res.end(), (uint8_t *) &size);
        assert(size < res.size());
        res.resize(size);

        return res;
    }

    std::vector<uint8_t> EncryptOAEP(const std::vector<uint8_t> &message, const PublicKey &key) {
        uint64_t block_size = (key.n.BitSize() + 7) / 8 * 8 - padding;
        auto data = message;
        while ((data.size() + sizeof(uint64_t)) % ((block_size - k0 - k1) / 8)) {
            data.push_back(0);
        }
        uint64_t size = message.size();
        data.insert(data.end(), (uint8_t *) &size, ((uint8_t *) &size) + sizeof(uint64_t));
        std::vector<uint8_t> res;
        for (uint64_t offset = 0; offset < data.size(); offset += (block_size - k0 - k1) / 8) {
            std::vector<uint8_t> r(k0 / 8);
            for (uint64_t i = 0; i * 8 < k0; i++) {
                r[i] = rand() >> 2;
            }
            std::vector<uint8_t> X((block_size - k0) / 8);
            std::copy(data.begin() + offset, data.begin() + offset + (block_size - k0 - k1) / 8, X.begin());
            std::vector<uint8_t> G = hash(r, block_size - k0);
            assert(X.size() == G.size());
            for (uint64_t i = 0; i < X.size(); i++) X[i] ^= G[i];
            std::vector<uint8_t> Y = hash(X, k0);
            assert(Y.size() == r.size());
            for (uint64_t i = 0; i < Y.size(); i++) Y[i] ^= r[i];
            std::vector<uint8_t> block = X;
            block.insert(block.end(), Y.begin(), Y.end());
            assert(block.size() == block_size / 8);
            UHugeInt number = UHugeInt::FromBytes(block);
            UHugeInt encoded_number = RSA::Encrypt(number, key);
            std::vector<uint8_t> encoded_bytes = encoded_number.ToBytes();
            assert(encoded_bytes.size() <= (block_size + padding) / 8);
            while (encoded_bytes.size() < (block_size + padding) / 8) {
                encoded_bytes.push_back(0);
            }
            res.insert(res.end(), encoded_bytes.begin(), encoded_bytes.end());
        }
        return res;
    }

    PrivateKey::PrivateKey(const UHugeInt &p, const UHugeInt &q, const UHugeInt &d) : p(p), q(q), d(d) {
        dp = d % (p - 1);
        dq = d % (q - 1);
        q_inv = InverseModulo(q, p);
    }

    PublicKey::PublicKey(const UHugeInt &n, const UHugeInt &e) : n(n), e(e) {

    }
}
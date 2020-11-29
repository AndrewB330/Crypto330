#include <gtest/gtest.h>
#include <crypto330/hugeint/hugeint.hpp>
#include <crypto330/symmetric/elliptic.hpp>
#include <crypto330/utils.hpp>

// ELLIPTIC CURVE PARAMETERS
HugePolyF2 MOD = HugePolyF2({0, 2, 4, 8, 307});
HugePolyF2 A = HugePolyF2::FromHex("1");
HugePolyF2 B = HugePolyF2::FromHex("393C7F7D53666B5054B5E6C6D3DE94F4296C0C599E2E2E241050DF18B6090BDC90186904968BB");
HugePolyF2 X = HugePolyF2::FromHex("216EE8B189D291A0224984C1E92F1D16BF75CCD825A087A239B276D3167743C52C02D6E7232AA");
HugePolyF2 Y = HugePolyF2::FromHex("5D9306BACD22B7FAEB09D2E049C6E2866C5D1677762A8F2F2DC9A11C7F7BE8340AB2237C7F2A0");
UHugeInt N = UHugeInt::FromHex("3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC079C2F3825DA70D390FBBA588D4604022B7B7");


TEST(Elliptic, Operations) {
    EllipticCurve curve = EllipticCurve(MOD, A, B, X, Y, N);
    std::mt19937_64 rng;

    auto P = curve.GeneratePoint(rng);
    auto Q = P + P;
    auto C = P + Q;
    auto U = C * 330;
    auto V = P * 921 + U;
    EXPECT_TRUE(Q.CheckOnCurve());
    EXPECT_TRUE(C.CheckOnCurve());
    EXPECT_TRUE(U.CheckOnCurve());
    EXPECT_TRUE(V.CheckOnCurve());
}

TEST(Elliptic, Signature) {
    EllipticCurve curve = EllipticCurve(MOD, A, B, X, Y, N);
    std::mt19937_64 rng;

    auto message = UHugeInt::FromBytes(StringToBytes("This is my message! And only my!"));
    auto [key_private, key_public] = EllipticSignature::GenerateKeys(curve, rng);
    auto signature = EllipticSignature::CreateSignature(message, key_private, curve, rng);
    EXPECT_TRUE(EllipticSignature::CheckSignature(message, signature, key_public, curve));
}

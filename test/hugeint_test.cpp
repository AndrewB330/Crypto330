#include <gtest/gtest.h>
#include <crypto330/hugeint/hugeint.hpp>

TEST(HugeInt, Stress) {
    std::vector<uint64_t> numbers;
    numbers.push_back(0);
    for (uint64_t i = 0; i < 48; i++) {
        numbers.push_back(1ull << i);
        numbers.push_back((1ull << i) + 1);
        numbers.push_back((1ull << i) - 1);
    }
    numbers.push_back(1000000007);
    numbers.push_back(1000000007ull * 1583);
    numbers.push_back(999999);
    numbers.push_back(10);
    numbers.push_back(1583);

    for (uint64_t i = 0; i < 100; i++) {
        numbers.push_back(rand() * rand());
    }

    uint64_t c = 0;
    for (uint64_t a : numbers) {
        for (uint64_t b : numbers) {
            UHugeInt A = UHugeInt(a);
            UHugeInt B = UHugeInt(b);
            c = (c+1) % 35;
            EXPECT_EQ(a + b, (A + B).ToUint64());
            if (a >= b) {
                EXPECT_EQ(a - b, (A - B).ToUint64());
            } else {
                EXPECT_EQ(0, (A - B).ToUint64());
            }
            EXPECT_EQ(a * b, (A * B).ToUint64());
            EXPECT_EQ(a << c, (A << c).ToUint64());
            EXPECT_EQ(a >> c, (A >> c).ToUint64());
            EXPECT_EQ(a < b, A < B);
            EXPECT_EQ(a > b, A > B);
            EXPECT_EQ(a >= b, A >= B);
            EXPECT_EQ(a <= b, A <= B);
            EXPECT_EQ(a == b, A == B);
            EXPECT_EQ(a != b, A != B);
            if (b) {
                EXPECT_EQ(a / b, (A / B).ToUint64());
                EXPECT_EQ(a % b, (A % B).ToUint64());
            }
            if (b && (a+b)*1.0*b+b < 18e18) {
                a += b;
                a *= b;
                a += b;

                A += B;
                A *= B;
                A += B;

                a /= b;
                A /= B;
                EXPECT_EQ(A.ToUint64(), a);
            }

        }
    }
}

TEST(HugeInt, Pow) {
    EXPECT_EQ(1, PowMod(UHugeInt(331), UHugeInt(0), UHugeInt(100)).ToUint64());
    EXPECT_EQ(111, PowMod(UHugeInt(331), UHugeInt(1), UHugeInt(220)).ToUint64());
    EXPECT_EQ(91, PowMod(UHugeInt(331), UHugeInt(1583), UHugeInt(100)).ToUint64());
    EXPECT_EQ(968334795, PowMod(UHugeInt(1583), UHugeInt(329), UHugeInt(1000000007)).ToUint64());
}
#include <crypto330/hugeint/math.hpp>
#include <cassert>
#include <iostream>

std::vector<uint64_t> SMALL_PRIMES = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73,
                                      79,
                                      83, 89, 97, 101, 103, 107, 109};

void Extended(UHugeInt a, UHugeInt b, UHugeInt &x, UHugeInt &y, bool &x_neg, bool &y_neg) {
    if (a == 0) {
        x = UHugeInt(0);
        y = UHugeInt(1);
        x_neg = false;
        y_neg = false;
        return;
    }
    UHugeInt x1, y1;
    bool x1_neg, y1_neg;
    Extended(b % a, a, x1, y1, x1_neg, y1_neg);

    y = x1;

    if (x1_neg ^ y1_neg) {
        x = y1 + (b / a) * x1;
        x_neg = y1_neg;
        y_neg = x1_neg;
    } else {
        if ((b / a) * x1 > y1) {
            x = (b / a) * x1 - y1;
            x_neg = true;
        } else {
            x = y1 - (b / a) * x1;
            x_neg = false;
        }
        y_neg = false;
    }
}

UHugeInt InverseModulo(const UHugeInt &a, const UHugeInt &mod) {
    UHugeInt x, y;
    bool xn, yn;
    Extended(a, mod, x, y, xn, yn);
    assert(x < mod);
    return xn ? mod - x : x;
}

bool IsProbablePrime(const UHugeInt &number, uint64_t tests) {
    if (number.IsZero() || !number.IsOdd() && number > 2) {
        return false;
    }
    for (uint64_t prime : SMALL_PRIMES) {
        if (number % prime == 0) {
            return number == prime;
        }
    }
    // Miller-Rabin test
    UHugeInt d = number - 1;
    uint64_t r = 0;
    while (!d.IsOdd()) {
        d >>= 1;
        r++;
    }

    std::mt19937_64 rng;
    rng.seed(number.ToUint64());

    for (uint64_t test = 0; test < tests; test++) {
        UHugeInt a = UHugeInt::Rand(2, number - 2, rng);
        UHugeInt x = UHugeInt::PowMod(a, d, number);
        if (x == 1 || x == number - 1) {
            continue;
        }
        bool found = true;
        for (uint64_t i = 0; i < r - 1; i++) {
            x = x * x % number;
            if (x == number - 1) {
                found = false;
                break;
            }
        }
        if (found) {
            return false;
        }
    }
    return true;
}

UHugeInt GreatestCommonDivisor(UHugeInt a, UHugeInt b) {
    while (!b.IsZero()) {
        a %= b;
        std::swap(a, b);
    }
    return a;
}
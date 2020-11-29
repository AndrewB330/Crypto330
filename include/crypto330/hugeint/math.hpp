#pragma once

#include "hugeint.hpp"

UHugeInt InverseModulo(const UHugeInt & a, const UHugeInt & mod);

HugePolyF2 InverseModulo(const HugePolyF2 & a, const HugePolyF2 & mod);

bool IsProbablePrime(const UHugeInt & number, uint64_t tests = 16);

UHugeInt GreatestCommonDivisor(UHugeInt a, UHugeInt b);

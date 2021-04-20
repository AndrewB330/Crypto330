#pragma once

#include <crypto330/hugeint/hugeint.hpp>
#include <crypto330/hugeint/math.hpp>

class EllipticCurve;

class EllipticCurvePoint {
public:
    EllipticCurvePoint(const HugePolyF2 &x, const HugePolyF2 &y, const EllipticCurve *curve);

    EllipticCurvePoint(const EllipticCurve *curve);

    HugePolyF2 x;
    HugePolyF2 y;

    bool IsZero() const;

    bool operator==(const EllipticCurvePoint &other) const;

    bool operator!=(const EllipticCurvePoint &other) const;

    EllipticCurvePoint operator+(const EllipticCurvePoint &other) const;

    EllipticCurvePoint operator*(UHugeInt other) const;

    bool CheckOnCurve() const;

private:
    bool is_zero;
    const EllipticCurve *curve;
};

class EllipticCurve {
public:
    EllipticCurve(HugePolyF2 mod, HugePolyF2 A, HugePolyF2 B, HugePolyF2 x, HugePolyF2 y, UHugeInt n);

    EllipticCurvePoint GetG() const;

    UHugeInt GetN() const;

    EllipticCurvePoint GeneratePoint(std::mt19937_64 &rng) const;

    friend class EllipticCurvePoint;

private:
    HugePolyF2 mod; // modulo
    HugePolyF2 A, B; // y^2 + xy = x^3 + Ax^2 + B
    EllipticCurvePoint g; // generator point
    UHugeInt n; // generator point period (g*(n+1) = g)
};

namespace EllipticSignature {
    struct PrivateKey {
        UHugeInt x;
    };

    struct PublicKey {
        EllipticCurvePoint Q;
    };

    struct Signature {
        UHugeInt r;
        UHugeInt s;
    };

    std::pair<PrivateKey, PublicKey> GenerateKeys(const EllipticCurve &curve, std::mt19937_64 &rng);

    Signature CreateSignature(const UHugeInt & message, const PrivateKey & key, const EllipticCurve & curve, std::mt19937_64 &rng);

    bool CheckSignature(const UHugeInt &message, const Signature & sign, const PublicKey &key, const EllipticCurve &curve);

}
#include <crypto330/symmetric/elliptic.hpp>
#include <cassert>
#include <crypto330/hugeint/math.hpp>

EllipticCurvePoint::EllipticCurvePoint(const HugePolyF2 &x, const HugePolyF2 &y, const EllipticCurve *curve)
        : x(x), y(y), curve(curve), is_zero(false) {}

EllipticCurvePoint::EllipticCurvePoint(const EllipticCurve *curve) : is_zero(true), curve(curve) {}

bool EllipticCurvePoint::IsZero() const {
    return is_zero;
}

bool EllipticCurvePoint::operator==(const EllipticCurvePoint &other) const {
    return x == other.x && y == other.y && curve == other.curve;
}

bool EllipticCurvePoint::operator!=(const EllipticCurvePoint &other) const {
    return !(*this == other);
}

EllipticCurvePoint EllipticCurvePoint::operator+(const EllipticCurvePoint &other) const {
    assert(curve == other.curve);
    if (IsZero()) {
        return other;
    }
    if (other.IsZero()) {
        return *this;
    }
    HugePolyF2 k;
    if (x == other.x) {
        k = (x * x % curve->mod + y) * InverseModulo(x, curve->mod) % curve->mod;
    } else {
        k = (y + other.y) * InverseModulo(x + other.x, curve->mod) % curve->mod;
    }
    HugePolyF2 x3 = (k * k + k + curve->A + x + other.x) % curve->mod;
    HugePolyF2 y3 = ((x + x3) * k + x3 + y) % curve->mod;
    return EllipticCurvePoint(x3, y3, curve);
}

EllipticCurvePoint EllipticCurvePoint::operator*(UHugeInt other) const {
    EllipticCurvePoint res = EllipticCurvePoint(curve);
    EllipticCurvePoint tmp = *this;
    while (!other.IsZero()) {
        if (other.IsOdd()) {
            res = res + tmp;
        }
        tmp = tmp + tmp;
        other = other >> 1;
    }
    return res;
}

bool EllipticCurvePoint::CheckOnCurve() const {
    HugePolyF2 x2 = (x*x) % curve->mod;
    return (y * y + x * y) % curve->mod == (x2 * x + curve->A * x2 + curve->B) % curve->mod;
}

EllipticCurve::EllipticCurve(HugePolyF2 mod, HugePolyF2 A, HugePolyF2 B, HugePolyF2 x, HugePolyF2 y, UHugeInt n)
        : mod(mod), A(A), B(B), g(EllipticCurvePoint(x, y, this)), n(n) {}

EllipticCurvePoint EllipticCurve::GetG() const {
    return g;
}

UHugeInt EllipticCurve::GetN() const {
    return n;
}

EllipticCurvePoint EllipticCurve::GeneratePoint(std::mt19937_64 &rng) const {
    return GetG() * UHugeInt::Rand(1, GetN() - 1, rng);
}

using namespace EllipticSignature;

std::pair<PrivateKey, PublicKey> GenerateKeys(const EllipticCurve &curve, std::mt19937_64 &rng) {
    auto x = UHugeInt::Rand(1, curve.GetN() - 1, rng);
    auto Q = curve.GetG() * x;
    return {PrivateKey{x}, PublicKey{Q}};
}

Signature CreateSignature(const UHugeInt & message, const PrivateKey & key, const EllipticCurve & curve, std::mt19937_64 &rng) {
    auto q = curve.GetN();
    auto k = UHugeInt::Rand(1, q, rng);
    auto r = (curve.GetG() * k).x.ToUHugeInt() % q;
    return {r, (InverseModulo(k, q) * (message + key.x * r)) % q};
}

bool CheckSignature(const UHugeInt &message, const Signature & sign, const PublicKey &key, const EllipticCurve &curve) {
    auto q = curve.GetN();
    auto u1 = InverseModulo(sign.s, q) * message % q;
    auto u2 = InverseModulo(sign.s, q) * sign.r % q;
    return (curve.GetG() * u1 + key.Q * u2).x.ToUHugeInt() == sign.r;
}
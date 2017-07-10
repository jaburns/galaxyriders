#include "fixed32.hpp"

#include <cmath>

static const int32_t DECIMAL_BITS = 16;
static const fixed32 FOUR_FACTORIAL = fixed32(24);
static const fixed32 SIX_FACTORIAL = fixed32(720);

const fixed32 fixed32::ZERO = fixed32(0);
const fixed32 fixed32::ONE = fixed32(1);
const fixed32 fixed32::MINUS_ONE = fixed32(-1);
const fixed32 fixed32::TWO = fixed32(2);
const fixed32 fixed32::TWO_PI = fixed32::from_raw_int(411774); // fixed32::from_float(6.283185307179586).to_raw_int();
const fixed32 fixed32::PI = TWO_PI / TWO;
const fixed32 fixed32::PI_OVER_TWO = PI / TWO;

fixed32::fixed32() 
{
    m_int = 0;
}

fixed32::fixed32(int16_t init) 
{
    m_int = init << DECIMAL_BITS;
}

fixed32 fixed32::from_raw_int(int32_t raw) 
{
    fixed32 ret;
    ret.m_int = raw;
    return ret;
}

fixed32 fixed32::from_fraction(int32_t numerator, int32_t denominator)
{
    return from_raw_int(static_cast<int16_t>((numerator << DECIMAL_BITS) / denominator));
}

int32_t fixed32::to_raw_int() const
{
    return m_int;
}

fixed32 fixed32::from_float(float x)
{
    return fixed32::from_raw_int(static_cast<int32_t>(x * 65536.0f));
}

float fixed32::to_float() const
{
    return static_cast<float>(m_int) / 65536.0f;
}

glm::vec2 fixed32::to_float(vec2 v)
{
    return { v.x.to_float(), v.y.to_float() };
}

glm::vec3 fixed32::to_float(vec3 v)
{
    return { v.x.to_float(), v.y.to_float(), v.z.to_float() };
}

fixed32::vec2 fixed32::from_float(glm::vec2 v)
{
    return { from_float(v.x), from_float(v.y) };
}

fixed32::vec3 fixed32::from_float(glm::vec3 v)
{
    return { from_float(v.x), from_float(v.y), from_float(v.z) };
}

fixed32& fixed32::operator +=(fixed32 rhs)
{
    m_int += rhs.m_int;
    return *this;
}

fixed32& fixed32::operator -=(fixed32 rhs)
{
    m_int -= rhs.m_int;
    return *this;
}

fixed32& fixed32::operator *=(fixed32 rhs)
{
    int64_t lhs64 = m_int;
    int64_t rhs64 = rhs.m_int;
    int64_t result = lhs64 * rhs64;
    m_int = static_cast<int32_t>(result >> DECIMAL_BITS);
    return *this;
}

fixed32& fixed32::operator /=(fixed32 rhs)
{
    int64_t lhs64 = m_int;
    int64_t rhs64 = rhs.m_int;
    int64_t result = (lhs64 << DECIMAL_BITS) / rhs64;
    m_int = static_cast<int32_t>(result);
    return *this;
}

fixed32& fixed32::operator %=(fixed32 rhs)
{
    m_int %= rhs.m_int;
    return *this;
}

fixed32 fixed32::operator +(fixed32 rhs) const { fixed32 ret = *this; ret += rhs; return ret; }
fixed32 fixed32::operator -(fixed32 rhs) const { fixed32 ret = *this; ret -= rhs; return ret; }
fixed32 fixed32::operator *(fixed32 rhs) const { fixed32 ret = *this; ret *= rhs; return ret; }
fixed32 fixed32::operator /(fixed32 rhs) const { fixed32 ret = *this; ret /= rhs; return ret; }
fixed32 fixed32::operator %(fixed32 rhs) const { fixed32 ret = *this; ret %= rhs; return ret; }

fixed32 fixed32::operator -() const { return from_raw_int(-m_int); }

bool fixed32::operator < (fixed32 rhs) const { return m_int <  rhs.m_int; }
bool fixed32::operator > (fixed32 rhs) const { return m_int >  rhs.m_int; }
bool fixed32::operator <=(fixed32 rhs) const { return m_int <= rhs.m_int; }
bool fixed32::operator >=(fixed32 rhs) const { return m_int >= rhs.m_int; }
bool fixed32::operator ==(fixed32 rhs) const { return m_int == rhs.m_int; }
bool fixed32::operator !=(fixed32 rhs) const { return m_int != rhs.m_int; }

fixed32 fixed32::abs() const
{
    return m_int < 0 ? -(*this) : *this;
}

fixed32 fixed32::sqrt() const
{
    fixed32 guess = *this / TWO;
    for (auto i = 0; i < 10; ++i) {
        guess = (guess + *this / guess) / TWO;
    }
    return guess;
}

fixed32 fixed32::sin() const
{
    return (*this - PI_OVER_TWO).cos();
}

fixed32 fixed32::cos() const
{
    auto x = *this;
    auto negate = false;

    x %= TWO_PI;

    if (x < -PI_OVER_TWO) {
        x += PI;
        negate = true;
    } else if (x > PI_OVER_TWO) {
        x -= PI;
        negate = true;
    }

    const auto x2 = x * x;
    const auto x4 = x2 * x2;
    const auto x6 = x4 * x2;
    const auto ret = ONE - x2 / TWO + x4 / FOUR_FACTORIAL - x6 / SIX_FACTORIAL;
    return negate ? -ret : ret;
}
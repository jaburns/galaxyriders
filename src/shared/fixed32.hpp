#pragma once

#include <cstdint>

// #include <glm/vec2.hpp>
// typedef glm::tvec2<fixed32> vec2f;

class fixed32 
{
    int32_t m_int = 0;

public:
    static const fixed32 ONE;
    static const fixed32 MINUS_ONE;
    static const fixed32 TWO;
    static const fixed32 TWO_PI; // fixed32::from_float(6.283185307179586).to_raw_int();
    static const fixed32 PI;
    static const fixed32 PI_OVER_TWO;

    fixed32();
    fixed32(int16_t init);

    static fixed32 from_raw_int(int32_t raw);
    static fixed32 from_fraction(int32_t numerator, int32_t denominator);
    static fixed32 from_float(float x);
    int32_t to_raw_int() const;
    float to_float() const;

    fixed32& operator +=(fixed32 rhs);
    fixed32& operator -=(fixed32 rhs);
    fixed32& operator *=(fixed32 rhs);
    fixed32& operator /=(fixed32 rhs);

    fixed32 operator +(fixed32 rhs) const;
    fixed32 operator -(fixed32 rhs) const;
    fixed32 operator *(fixed32 rhs) const;
    fixed32 operator /(fixed32 rhs) const;

    fixed32 operator -() const;

    bool operator < (fixed32 rhs) const;
    bool operator > (fixed32 rhs) const;
    bool operator <=(fixed32 rhs) const;
    bool operator >=(fixed32 rhs) const;
    bool operator ==(fixed32 rhs) const;
    bool operator !=(fixed32 rhs) const;

    fixed32 sqrt() const;
    fixed32 sin() const;
    fixed32 cos() const;
};

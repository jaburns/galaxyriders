#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class fixed32
{
    int32_t m_int = 0;

public:
    static const fixed32 ZERO;
    static const fixed32 ONE;
    static const fixed32 MINUS_ONE;
    static const fixed32 TWO;
    static const fixed32 TWO_PI;
    static const fixed32 PI;
    static const fixed32 PI_OVER_TWO;

    fixed32();
    fixed32(int16_t init);

    static fixed32 from_raw_int(int32_t raw);
    static fixed32 from_fraction(int32_t numerator, int32_t denominator);
    static fixed32 from_float(float x);
    int32_t to_raw_int() const;
    float to_float() const;

    typedef glm::tvec2<fixed32> vec2;
    typedef glm::tvec3<fixed32> vec3;

    static glm::vec2 to_float(vec2 v);
    static glm::vec3 to_float(vec3 v);
    static vec2 from_float(glm::vec2 v);
    static vec3 from_float(glm::vec3 v);

    fixed32& operator +=(fixed32 rhs);
    fixed32& operator -=(fixed32 rhs);
    fixed32& operator *=(fixed32 rhs);
    fixed32& operator /=(fixed32 rhs);
    fixed32& operator %=(fixed32 rhs);

    fixed32 operator +(fixed32 rhs) const;
    fixed32 operator -(fixed32 rhs) const;
    fixed32 operator *(fixed32 rhs) const;
    fixed32 operator /(fixed32 rhs) const;
    fixed32 operator %(fixed32 rhs) const;

    fixed32 operator -() const;

    bool operator < (fixed32 rhs) const;
    bool operator > (fixed32 rhs) const;
    bool operator <=(fixed32 rhs) const;
    bool operator >=(fixed32 rhs) const;
    bool operator ==(fixed32 rhs) const;
    bool operator !=(fixed32 rhs) const;

    fixed32 abs() const;
    fixed32 sqrt() const;
    fixed32 sin() const;
    fixed32 cos() const;
    fixed32 pow(int32_t p) const;

    static fixed32 max(fixed32 a, fixed32 b);

    static fixed32 length(const vec2& v);
    static vec2 normalize(const vec2& v);
    static vec2 rotate90(const vec2& v);
    static fixed32 dot(const fixed32::vec2& a, const fixed32::vec2& b);
    static fixed32 cross(const fixed32::vec2& a, const fixed32::vec2& b);
    static vec2 reflect(const vec2& v, const vec2& unit_normal, fixed32 normal_scale, fixed32 tangent_scale);
};

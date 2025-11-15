#pragma once

#include <utility>
#include <cmath>
#include <simd/simd.h>

struct CircleFactory
{
    static constexpr float pi = 3.1415926f;

    float const radius;
    int const quality;
    float const deltaAngle;

    CircleFactory(float radius_, int quality_)
        : radius{radius_},
          quality{quality_},
          deltaAngle{(2.0f * pi) / static_cast<float>(quality_)}
    {
    }

    // Returns the x,y point on the circle perimeter for a given vertex index
    // Index wraps around quality; negative indices are supported
    simd::float2 getPoint(int index) const
    {
        if (quality <= 0)
        {
            return {0.0f, 0.0f};
        }

        int i = index % quality;
        if (i < 0)
            i += quality; // handle negative indices

        float angle = static_cast<float>(i) * deltaAngle;
        float x = std::cos(angle) * radius;
        float y = std::sin(angle) * radius;
        return {x, y};
    }
};
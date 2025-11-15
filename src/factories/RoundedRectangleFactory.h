#pragma once

#include <simd/simd.h>
#include "factories/CircleFactory.h"

struct RoundedRectangleFactory
{
    // Size of the rectangle (width, height) in local space starting at (0,0)
    simd::float2 const size;
    // Radius for all corners (clamped by caller to fit size)
    float const radius;
    // Number of segments per corner (>= 1). Total perimeter sample count is 4*perCorner.
    int const perCorner;
    // Circle sampler with quality = 4 * perCorner so that each corner spans a contiguous quarter of indices
    CircleFactory const factory;

    // Corner centers in clockwise order starting from Top-Right, then Bottom-Right, Bottom-Left, Top-Left
    simd::float2 const centers[4];

    // qualityPerCorner specifies how many samples per 90-degree arc (excluding the shared endpoint with the next arc)
    RoundedRectangleFactory(simd::float2 size_, float radius_, int qualityPerCorner)
        : size{size_},
          radius{radius_},
          perCorner{qualityPerCorner < 1 ? 1 : qualityPerCorner},
          factory{radius, 4 * (qualityPerCorner < 1 ? 1 : qualityPerCorner)},
          centers{
              simd::float2{size_.x - radius_, radius_},          // Top-Right
              simd::float2{size_.x - radius_, size_.y - radius_},// Bottom-Right
              simd::float2{radius_,          size_.y - radius_}, // Bottom-Left
              simd::float2{radius_,          radius_}            // Top-Left
          }
    {}

    // Total number of unique perimeter points (excluding the final wrap point)
    int pointCount() const { return 4 * perCorner; }

    // Returns perimeter point in clockwise order, starting at the top edge near the top-right corner.
    // Index wraps automatically.
    simd::float2 getPoint(int index) const
    {
        int N = pointCount();
        if (N <= 0) return {0.0f, 0.0f};
        int i = index % N;
        if (i < 0) i += N;

        int corner = i / perCorner;            // 0..3 in TR,BR,BL,TL order
        int inCorner = i % perCorner;          // 0..perCorner-1

        // CircleFactory indices for a full circle partitioned into 4 quarters of size perCorner
        // We want the Top-Right quarter first, which is the 4th quarter in standard CCW order (indices 3*pc..4*pc-1)
        int base = ((corner + 3) % 4) * perCorner; // TR:3pc, BR:0, BL:pc, TL:2pc
        int circleIndex = base + inCorner;

        return centers[corner] + factory.getPoint(circleIndex);
    }
};
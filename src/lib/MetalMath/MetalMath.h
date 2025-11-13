# pragma once

#include <simd/simd.h>

namespace MetalMath
{
    simd::float4x4 identity();
    simd::float4x4 translate(simd::float3 positionDelta);
    simd::float4x4 rotateZ(float theta);
    simd::float4x4 scale(float factor);
}
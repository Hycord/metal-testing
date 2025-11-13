#pragma once

#include <simd/simd.h>

namespace MetalMath
{
    simd::float4x4 identity();
    simd::float4x4 translate(simd::float3 positionDelta);
    simd::float4x4 rotateZ(float theta);
    simd::float4x4 scale(float factor);
    simd::float4x4 perspectiveProjection(float fovY, float aspectRatio, float near, float far);

    simd::float4x4 cameraView(simd::float3 right, simd::float3 up, simd::float3 forwards, simd::float3 pos);
}
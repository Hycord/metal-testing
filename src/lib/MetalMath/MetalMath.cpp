#include "MetalMath.h"
#include <math.h>

simd::float4x4 MetalMath::identity()
{
    simd_float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};

    return simd_matrix(col0, col1, col2, col3);
};

simd::float4x4 MetalMath::translate(simd::float3 positionDelta)
{

    simd_float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    simd_float4 col3 = {positionDelta[0], positionDelta[1], positionDelta[2], 1.0f};

    return simd_matrix(col0, col1, col2, col3);
};

simd::float4x4 MetalMath::rotateZ(float theta)
{

    theta = theta * M_PI / 180.0f;
    float cosTheta = cosf(theta);
    float sinTheta = sinf(theta);

    simd_float4 col0 = {cosTheta, sinTheta, 0.0f, 0.0f};
    simd_float4 col1 = {-sinTheta, cosTheta, 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};

    return simd_matrix(col0, col1, col2, col3);
};

simd::float4x4 MetalMath::scale(float factor)
{

    simd_float4 col0 = {factor, 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, factor, 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, factor, 0.0f};
    simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};

    return simd_matrix(col0, col1, col2, col3);
};

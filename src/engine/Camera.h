#pragma once

#include "utils/Math.h"

enum class ProjectionType {
    Perspective,
    Orthographic,
    Custom
};

struct CameraMatrices {
    ProjectionType projectionType = ProjectionType::Perspective;
    simd::float4x4 view = MetalMath::identity();
    simd::float4x4 projection = MetalMath::identity();
};

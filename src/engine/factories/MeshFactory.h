#pragma once

#include "engine/config.h"

namespace MeshFactory
{
    MTL::Buffer *buildTriangle(MTL::Device *device);
    Mesh buildQuad(MTL::Device *device);
    Mesh buildCube(MTL::Device *device);
    Mesh buildScreenQuad(MTL::Device *device, float left, float top, float width, float height);
};

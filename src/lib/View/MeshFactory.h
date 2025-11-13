#pragma once

#include "../../config.h"

namespace MeshFactory
{
    MTL::Buffer *buildTriangle(MTL::Device *device);
    Mesh buildQuad(MTL::Device *device);
};

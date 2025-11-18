#pragma once

#include "engine/components/engine/Renderable.h"

class WorldContainer : public Renderable {
public:
    WorldContainer();
    virtual ~WorldContainer();

    virtual void render(MTL::RenderCommandEncoder* encoder,
                       const simd::float4x4& projection,
                       const simd::float4x4& view);
};

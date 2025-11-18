#pragma once

#include "engine/components/engine/Renderable.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include <memory>



class RoundedRectanglePrimitive : public RenderablePrimitive {
public:
    
    RoundedRectanglePrimitive(MTL::Device* device,
                                float left, float top,
                                float width, float height,
                                float radius,
                                const simd::float4 &col,
                                int qualityPerCorner = 6);

    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;

    void setPosition(float left, float top) { l = left; t = top; dirty = true; }
    void setSize(float w, float h) { width = w; height = h; dirty = true; }
    void setRadius(float r) { rad = r; dirty = true; }
    void setQualityPerCorner(int q) { perCorner = q < 1 ? 1 : q; dirty = true; }

private:
    void rebuild();

    MTL::Device* device;
    float l, t, width, height;
    float rad;
    int perCorner;
    bool dirty = true;
    Mesh mesh{};
    std::shared_ptr<Renderable> renderable;

    void onColorChanged() override;
};

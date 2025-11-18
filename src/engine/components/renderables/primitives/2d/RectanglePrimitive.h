#pragma once

#include "engine/components/engine/Renderable.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include <memory>



class RectanglePrimitive : public RenderablePrimitive {
public:
    RectanglePrimitive(MTL::Device* device, float left, float top, float width, float height, const simd::float4 &col);
    void draw(MTL::RenderCommandEncoder* encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;
    void setPosition(float left, float top) { l = left; t = top; dirty = true; }
    void setSize(float w, float h) { width = w; height = h; dirty = true; }
private:
    void ensureMesh();
    MTL::Device* device;
    float l, t, width, height;
    bool dirty = true;
    Mesh mesh{};
    std::shared_ptr<Renderable> renderable;

    void onColorChanged() override;
};

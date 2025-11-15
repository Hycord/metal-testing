#pragma once

#include "components/engine/Renderable.h"
#include "components/renderables/primitives/UIPrimitive.h"

// Rectangle primitive positioned by top-left corner in pixel space.
class RectangleUIPrimitive : public UIPrimitive {
public:
    RectangleUIPrimitive(MTL::Device* device, float left, float top, float width, float height, const simd::float4 &col);
    void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) override;
    void setPosition(float left, float top) { l = left; t = top; dirty = true; }
    void setSize(float w, float h) { width = w; height = h; dirty = true; }
private:
    void ensureMesh();
    MTL::Device* device;
    float l, t, width, height;
    bool dirty = true;
    Mesh mesh{};
    std::unique_ptr<Renderable> renderable;
};
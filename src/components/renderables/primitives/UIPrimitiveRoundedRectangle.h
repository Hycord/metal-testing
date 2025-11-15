#pragma once

#include "components/engine/Renderable.h"
#include "components/renderables/primitives/UIPrimitive.h"

// Rounded rectangle primitive positioned by top-left corner in pixel space.
class RoundedRectangleUIPrimitive : public UIPrimitive {
public:
    // qualityPerCorner: how many segments to approximate each 90-degree arc (>=1)
    RoundedRectangleUIPrimitive(MTL::Device* device,
                                float left, float top,
                                float width, float height,
                                float radius,
                                const simd::float4 &col,
                                int qualityPerCorner = 6);

    void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) override;

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
    std::unique_ptr<Renderable> renderable;
};

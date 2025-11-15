#pragma once

#include "components/engine/Renderable.h"
#include "components/renderables/primitives/UIPrimitive.h"

// Circle primitive centered at (cx, cy) with radius r. Approximated as triangle fan.
class CircleUIPrimitive : public UIPrimitive
{
public:
    CircleUIPrimitive(MTL::Device *device, float cx, float cy, float radius, const simd::float4 &col, int segments = 32);
    void draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &ortho) override;
    void setCenter(float x, float y)
    {
        cx = x;
        cy = y;
        dirty = true;
    }
    void setRadius(float r)
    {
        radius = r;
        dirty = true;
    }

private:
    void rebuild();
    MTL::Device *device;
    float cx, cy, radius;
    int segments;
    bool dirty = true;
    Mesh mesh{};
    std::unique_ptr<Renderable> renderable;
};

// Composite primitive to nest and compose multiple primitives
class CompositeUIPrimitive : public UIPrimitive
{
public:
    void add(const std::shared_ptr<UIPrimitive> &child) { children.push_back(child); }
    void clear() { children.clear(); }
    void draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &ortho) override
    {
        for (auto &c : children)
        {
            if (c)
                c->draw(encoder, ortho);
        }
    }

private:
    std::vector<std::shared_ptr<UIPrimitive>> children;
};

#pragma once

#include "engine/components/engine/Renderable.h"
#include "engine/components/renderables/primitives/RenderablePrimitive.h"
#include <memory>



class CirclePrimitive : public RenderablePrimitive
{
public:
    CirclePrimitive(MTL::Device *device, float cx, float cy, float radius, const simd::float4 &col, int segments = 32);
    void draw(MTL::RenderCommandEncoder *encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override;
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
    std::shared_ptr<Renderable> renderable;

    void onColorChanged() override;
};


class CompositePrimitive : public RenderablePrimitive
{
public:
    void add(const std::shared_ptr<RenderablePrimitive> &child) { children.push_back(child); }
    void clear() { children.clear(); }
    void draw(MTL::RenderCommandEncoder *encoder,
              const simd::float4x4 &projection,
              const simd::float4x4 &view) override
    {
        for (auto &c : children)
        {
            if (c)
                c->draw(encoder, projection, view);
        }
    }

private:
    std::vector<std::shared_ptr<RenderablePrimitive>> children;
};

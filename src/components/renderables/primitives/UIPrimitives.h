#pragma once

#include "components/engine/Renderable.h"
#include "components/engine/Material.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include <memory>
#include <vector>

// Base class for UI primitives that can be composed and drawn inside UIElements.
class UIPrimitive {
public:
    virtual ~UIPrimitive() {}
    virtual void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) = 0;
    void setPrimitiveType(MTL::PrimitiveType t) { primitiveType = t; }
    void setColor(const simd::float4 &c) { color = c; }
protected:
    // Helper to build a Renderable with provided mesh and color
    std::unique_ptr<Renderable> makeRenderable(MTL::Device* device, const Mesh &mesh, const simd::float4 &col, MTL::PrimitiveType primType) {
        Shader* shader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", mesh.vertexDescriptor);
        Material* material = new Material(shader);
        material->setColor(col);
        auto r = std::make_unique<Renderable>(mesh, material);
        r->setScreenSpace(true);
        r->setPrimitiveType(primType);
        return r;
    }
    MTL::PrimitiveType primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
    simd::float4 color {1,1,1,1};
};

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

// Circle primitive centered at (cx, cy) with radius r. Approximated as triangle fan.
class CircleUIPrimitive : public UIPrimitive {
public:
    CircleUIPrimitive(MTL::Device* device, float cx, float cy, float radius, const simd::float4 &col, int segments = 32);
    void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) override;
    void setCenter(float x, float y) { cx = x; cy = y; dirty = true; }
    void setRadius(float r) { radius = r; dirty = true; }
private:
    void rebuild();
    MTL::Device* device;
    float cx, cy, radius;
    int segments;
    bool dirty = true;
    Mesh mesh{};
    std::unique_ptr<Renderable> renderable;
};

// Composite primitive to nest and compose multiple primitives
class CompositeUIPrimitive : public UIPrimitive {
public:
    void add(const std::shared_ptr<UIPrimitive>& child) { children.push_back(child); }
    void clear() { children.clear(); }
    void draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho) override {
        for (auto &c : children) {
            if (c) c->draw(encoder, ortho);
        }
    }
private:
    std::vector<std::shared_ptr<UIPrimitive>> children;
};

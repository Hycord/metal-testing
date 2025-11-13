#pragma once

#include "components/renderables/core/UIContainer.h"
#include "config.h"
#include "components/renderables/primitives/UIPrimitives.h"
#include <memory>
#include <vector>

class UIElement : public UIContainer {
public:
    UIElement(MTL::Device* device);
    virtual ~UIElement();

    // Draw the cached quad if present
    void drawCachedQuad(MTL::RenderCommandEncoder* encoder);
    // Draw children primitives attached to this UI element (default: draw composed primitives list)
    virtual void drawPrimitives(MTL::RenderCommandEncoder* encoder);

    // Manage composed primitives
    void addPrimitive(const std::shared_ptr<UIPrimitive>& prim);
    void clearPrimitives();

protected:
    // Helper to build a cached screen-space quad (left, top in pixel coords)
    void buildCachedQuad(float left, float top, float width, float height, const simd::float4 &color);
    void destroyCachedQuad();
    // Update the position of an existing cached quad without recreating shaders/materials.
    // This updates the vertex buffer in-place and is cheap compared to rebuild.
    void moveCachedQuad(float left, float top, float width, float height);

    MTL::Device* device;

    // cached resources for a simple colored quad
    Mesh quadMesh;
    Shader* quadShader;
    Material* quadMaterial;
    Renderable* quadRenderable;
    std::vector<std::shared_ptr<UIPrimitive>> primitives;
    // Expose device to subclasses
    MTL::Device* getDevice() const { return device; }
};

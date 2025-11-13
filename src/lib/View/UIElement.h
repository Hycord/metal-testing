#pragma once

#include "UIContainer.h"
#include "../../config.h"

class UIElement : public UIContainer {
public:
    UIElement(MTL::Device* device, float screenWidth, float screenHeight);
    virtual ~UIElement();

    // Called when drawable/window size changes
    void setScreenSize(float screenWidth, float screenHeight);

    // Draw the cached quad if present
    void drawCachedQuad(MTL::RenderCommandEncoder* encoder);

protected:
    // Derived classes override to respond to size changes (rebuild meshes/resources)
    virtual void onSizeChanged();

    // Helper to build a cached screen-space quad (left, top in pixel coords)
    void buildCachedQuad(float left, float top, float width, float height, const simd::float4 &color);
    void destroyCachedQuad();
    // Update the position of an existing cached quad without recreating shaders/materials.
    // This updates the vertex buffer in-place and is cheap compared to rebuild.
    void moveCachedQuad(float left, float top, float width, float height);

    MTL::Device* device;
    float screenWidth;
    float screenHeight;

    // cached resources for a simple colored quad
    Mesh quadMesh;
    Shader* quadShader;
    Material* quadMaterial;
    Renderable* quadRenderable;
};

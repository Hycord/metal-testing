#pragma once

#include "components/renderables/core/UIContainer.h"
#include "components/renderables/core/UITransform.h"
#include "config.h"
#include "components/renderables/primitives/RenderablePrimitive.h"
#include <memory>
#include <vector>

class UIElement : public UIContainer {
public:
    UIElement(MTL::Device* device);
    virtual ~UIElement();

    
    void drawCachedQuad(MTL::RenderCommandEncoder* encoder);
    
    virtual void drawPrimitives(MTL::RenderCommandEncoder* encoder);

    
    void addPrimitive(const std::shared_ptr<RenderablePrimitive>& prim);
    void clearPrimitives();

    UITransform& getTransform() { return transform; }
    const UITransform& getTransform() const { return transform; }
    
    enum class AnchorCorner {
        BottomLeft,
        BottomRight,
        TopLeft,
        TopRight
    };
    void enableAutoAnchor(AnchorCorner corner, float marginX, float marginY);
    void disableAutoAnchor();

    void updateSizeFromPrimitives();
    void getContentSize(float& width, float& height) const;

protected:
    
    void buildCachedQuad(float left, float top, float width, float height, const simd::float4 &color);
    void destroyCachedQuad();
    
    
    void moveCachedQuad(float left, float top, float width, float height);

    MTL::Device* device;

    UITransform transform;
    
    Mesh quadMesh;
    Shader* quadShader;
    Material* quadMaterial;
    Renderable* quadRenderable;
    std::vector<std::shared_ptr<RenderablePrimitive>> primitives;
    
    MTL::Device* getDevice() const { return device; }

    
    float elementWidth = 0.0f;
    float elementHeight = 0.0f;
    float cachedLeft = 0.0f;
    float cachedTop = 0.0f;
    bool autoAnchorEnabled = false;
    AnchorCorner anchorCorner = AnchorCorner::BottomLeft;
    float anchorMarginX = 0.0f;
    float anchorMarginY = 0.0f;
};

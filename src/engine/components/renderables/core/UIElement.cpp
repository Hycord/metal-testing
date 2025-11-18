#include "engine/components/renderables/core/UIElement.h"
#include "engine/factories/MeshFactory.h"
#include "engine/components/engine/Shader.h"
#include "engine/components/engine/Material.h"
#include "engine/components/engine/Renderable.h"
#include "engine/core/LogManager.h"
#include "engine/utils/Math.h"
#include "engine/systems/input/InputState.h"

UIElement::UIElement(MTL::Device* device)
    : device(device), quadShader(nullptr), quadMaterial(nullptr), quadRenderable(nullptr)
{
    transform.setSize(0.0f, 0.0f);
}

UIElement::~UIElement()
{
    destroyCachedQuad();
}

void UIElement::render(MTL::RenderCommandEncoder *encoder)
{
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    transform.update(screenWidth, screenHeight);
    
    drawPrimitives(encoder);
}

void UIElement::buildCachedQuad(float left, float top, float width, float height, const simd::float4 &color)
{
    LOG_START("UIElement: buildCachedQuad");
    destroyCachedQuad();

    transform.setSize(width, height);
    transform.setPosition(left, top);
    cachedLeft = left;
    cachedTop = top;
    elementWidth = width;
    elementHeight = height;

    quadMesh = MeshFactory::buildScreenQuad(device, left, top, width, height);
    quadShader = new Shader(device, "General", "vertexGeneral", "fragmentGeneral", quadMesh.vertexDescriptor);
    quadMaterial = new Material(quadShader);
    quadMaterial->setColor(color);
    quadRenderable = new Renderable(quadMesh, quadMaterial);
    quadRenderable->setScreenSpace(true);

    LOG_FINISH("UIElement: buildCachedQuad");
}

void UIElement::moveCachedQuad(float left, float top, float width, float height)
{
    
    
    if (!quadMesh.vertexBuffer || !quadRenderable) {
        LOG_ERROR("UIElement::moveCachedQuad called without valid quad - call buildCachedQuad first");
        return;
    }

    
    float l = left;
    float t = top;
    float r = left + width;
    float b = top + height;

    Vertex verticies[4] = {
        {{l, b, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{r, b, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{r, t, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{l, t, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    
    void* dst = quadMesh.vertexBuffer->contents();
    memcpy(dst, verticies, 4 * sizeof(Vertex));
}

void UIElement::destroyCachedQuad()
{
    if (quadRenderable) {
        delete quadRenderable; 
        quadRenderable = nullptr;
        quadMaterial = nullptr;
        quadShader = nullptr;
    } else {
        
        if (quadMaterial) {
            delete quadMaterial;
            quadMaterial = nullptr;
            quadShader = nullptr;
        }
        if (quadShader) {
            delete quadShader;
            quadShader = nullptr;
        }
    }
    
    quadMesh = {};
    transform.setSize(0.0f, 0.0f);
    elementWidth = 0.0f;
    elementHeight = 0.0f;
}

void UIElement::drawCachedQuad(MTL::RenderCommandEncoder* encoder)
{
    if (!quadRenderable) return;
    
    if (autoAnchorEnabled && elementWidth > 0.0f && elementHeight > 0.0f)
    {
        const float screenWidth = InputState::getWindowWidth();
        const float screenHeight = InputState::getWindowHeight();

        float left = cachedLeft;
        float top = cachedTop;
        switch (anchorCorner)
        {
            case AnchorCorner::BottomLeft:
                left = anchorMarginX;
                top = anchorMarginY;
                break;
            case AnchorCorner::BottomRight:
                left = screenWidth - elementWidth - anchorMarginX;
                top = anchorMarginY;
                break;
            case AnchorCorner::TopLeft:
                left = anchorMarginX;
                top = screenHeight - elementHeight - anchorMarginY;
                break;
            case AnchorCorner::TopRight:
                left = screenWidth - elementWidth - anchorMarginX;
                top = screenHeight - elementHeight - anchorMarginY;
                break;
        }
        if (left != cachedLeft || top != cachedTop)
        {
            moveCachedQuad(left, top, elementWidth, elementHeight);
            cachedLeft = left;
            cachedTop = top;
        }
    }
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    
    simd::float4x4 ortho;
    {
        simd_float4 col0 = {2.0f / (screenWidth - 0.0f), 0.0f, 0.0f, 0.0f};
        simd_float4 col1 = {0.0f, 2.0f / (screenHeight - 0.0f), 0.0f, 0.0f};
        simd_float4 col2 = {0.0f, 0.0f, 1.0f / (1.0f - -1.0f), 0.0f};
        simd_float4 col3 = {-(screenWidth + 0.0f) / (screenWidth - 0.0f), -(screenHeight + 0.0f) / (screenHeight - 0.0f), -(-1.0f) / (1.0f - -1.0f), 1.0f};
        ortho = simd_matrix(col0, col1, col2, col3);
    }
    simd::float4x4 identity = MetalMath::identity();
    quadRenderable->draw(encoder, ortho, identity);
    
    drawPrimitives(encoder);
}

void UIElement::addPrimitive(const std::shared_ptr<RenderablePrimitive>& prim)
{
    primitives.push_back(prim);
}

void UIElement::clearPrimitives()
{
    primitives.clear();
}

void UIElement::drawPrimitives(MTL::RenderCommandEncoder* encoder)
{
    if (primitives.empty()) return;
    const float screenWidth = InputState::getWindowWidth();
    const float screenHeight = InputState::getWindowHeight();
    simd::float4x4 ortho;
    {
        simd_float4 col0 = {2.0f / (screenWidth - 0.0f), 0.0f, 0.0f, 0.0f};
        simd_float4 col1 = {0.0f, 2.0f / (screenHeight - 0.0f), 0.0f, 0.0f};
        simd_float4 col2 = {0.0f, 0.0f, 1.0f / (1.0f - -1.0f), 0.0f};
        simd_float4 col3 = {-(screenWidth + 0.0f) / (screenWidth - 0.0f), -(screenHeight + 0.0f) / (screenHeight - 0.0f), -(-1.0f) / (1.0f - -1.0f), 1.0f};
        ortho = simd_matrix(col0, col1, col2, col3);
    }
    for (auto &p : primitives) {
        if (p) p->drawScreenSpace(encoder, ortho);
    }
}

void UIElement::enableAutoAnchor(AnchorCorner corner, float marginX, float marginY)
{
    anchorCorner = corner;
    anchorMarginX = marginX;
    anchorMarginY = marginY;
    autoAnchorEnabled = true;
    
    AnchorPoint point;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    
    switch (corner) {
        case AnchorCorner::BottomLeft:
            point = AnchorPoint::BottomLeft;
            offsetX = marginX;
            offsetY = marginY;
            break;
        case AnchorCorner::BottomRight:
            point = AnchorPoint::BottomRight;
            offsetX = -marginX;
            offsetY = marginY;
            break;
        case AnchorCorner::TopLeft:
            point = AnchorPoint::TopLeft;
            offsetX = marginX;
            offsetY = -marginY;
            break;
        case AnchorCorner::TopRight:
            point = AnchorPoint::TopRight;
            offsetX = -marginX;
            offsetY = -marginY;
            break;
    }
    
    transform.setAnchor(AnchorTarget::Screen, point, offsetX, offsetY);
}

void UIElement::disableAutoAnchor()
{
    autoAnchorEnabled = false;
    transform.clearAnchor();
}

void UIElement::updateSizeFromPrimitives()
{
    float maxWidth = 0.0f;
    float maxHeight = 0.0f;
    
    for (const auto& prim : primitives) {
        if (!prim) continue;
        
        float primWidth, primHeight;
        prim->getContentSize(primWidth, primHeight);
        
        if (primWidth > maxWidth) maxWidth = primWidth;
        if (primHeight > maxHeight) maxHeight = primHeight;
    }
    
    if (maxWidth > 0.0f && maxHeight > 0.0f) {
        elementWidth = maxWidth;
        elementHeight = maxHeight;
        transform.setSize(maxWidth, maxHeight);
        
        if (quadMesh.vertexBuffer && quadRenderable) {
            moveCachedQuad(cachedLeft, cachedTop, maxWidth, maxHeight);
        }
    }
}

void UIElement::getContentSize(float& width, float& height) const
{
    width = 0.0f;
    height = 0.0f;
    
    for (const auto& prim : primitives) {
        if (!prim) continue;
        
        float primWidth, primHeight;
        prim->getContentSize(primWidth, primHeight);
        
        if (primWidth > width) width = primWidth;
        if (primHeight > height) height = primHeight;
    }
}

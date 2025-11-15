#include "components/renderables/core/UIElement.h"
#include "factories/MeshFactory.h"
#include "components/engine/Shader.h"
#include "components/engine/Material.h"
#include "components/engine/Renderable.h"
#include "core/LogManager.h"
#include "utils/Math.h"
#include "systems/input/InputState.h"

UIElement::UIElement(MTL::Device* device)
    : device(device), quadShader(nullptr), quadMaterial(nullptr), quadRenderable(nullptr)
{
}

UIElement::~UIElement()
{
    destroyCachedQuad();
}

void UIElement::buildCachedQuad(float left, float top, float width, float height, const simd::float4 &color)
{
    LOG_START("UIElement: buildCachedQuad");
    destroyCachedQuad();

    // Remember size and initial position
    elementWidth = width;
    elementHeight = height;
    cachedLeft = left;
    cachedTop = top;

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
    // Update positions in the existing vertex buffer if present. If no buffer
    // exists, fall back to building a new cached quad.
    if (!quadMesh.vertexBuffer) {
        buildCachedQuad(left, top, width, height, simd::float4{1.0f,1.0f,1.0f,1.0f});
        return;
    }

    // Construct new vertex positions matching MeshFactory::buildScreenQuad layout
    float l = left;
    float t = top;
    float r = left + width;
    float b = top + height;

    Vertex verticies[4] = {
        {{l, b, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{r, b, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{r, t, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{l, t, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    // Update the vertex buffer contents in-place (shared storage mode expected)
    void* dst = quadMesh.vertexBuffer->contents();
    memcpy(dst, verticies, 4 * sizeof(Vertex));
}

void UIElement::destroyCachedQuad()
{
    if (quadRenderable) {
        delete quadRenderable; // Renderable destructor will release material and shader
        quadRenderable = nullptr;
    } else {
        // If quadRenderable was null, we still need to ensure shader/material are released if they exist
        if (quadMaterial) { delete quadMaterial; quadMaterial = nullptr; }
        if (quadShader) { delete quadShader; quadShader = nullptr; }
    }
    // quadMesh contains retained buffers; Renderable destructor would have released them. To be safe, zero it.
    quadMesh = {};
    elementWidth = 0.0f;
    elementHeight = 0.0f;
}

void UIElement::drawCachedQuad(MTL::RenderCommandEncoder* encoder)
{
    if (!quadRenderable) return;
    // Auto-anchor reposition, if enabled
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
    // Build an ortho projection mapping 0..screenWidth x 0..screenHeight
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
    // Draw any composed primitives relative to the same ortho
    drawPrimitives(encoder);
}

void UIElement::addPrimitive(const std::shared_ptr<UIPrimitive>& prim)
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
        if (p) p->draw(encoder, ortho);
    }
}

void UIElement::enableAutoAnchor(AnchorCorner corner, float marginX, float marginY)
{
    anchorCorner = corner;
    anchorMarginX = marginX;
    anchorMarginY = marginY;
    autoAnchorEnabled = true;
}

void UIElement::disableAutoAnchor()
{
    autoAnchorEnabled = false;
}

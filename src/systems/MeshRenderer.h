#pragma once

#include "config.h"
#include "engine/Camera.h"
#include "components/engine/Renderable.h"
#include "components/renderables/core/UIContainer.h"
#include <memory>
#include <vector>

class MeshRenderer {
public:
    MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer);
    ~MeshRenderer();

    void addRenderable(const std::shared_ptr<Renderable> &r);
    void clearRenderables();
    void draw(const CameraMatrices &camera, const std::vector<std::shared_ptr<UIContainer>> &uiElements);
    void drawUI(const std::vector<std::shared_ptr<UIContainer>> &uiElements, MTL::RenderCommandEncoder *encoder);

    MTL::CommandBuffer* createUICommandBuffer();
    MTL::RenderCommandEncoder* createUIEncoder(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable** outDrawable);
    void commitUICommandBuffer(MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* encoder, CA::MetalDrawable* drawable);

    void setOrthoParams(float left, float right, float bottom, float top, float near, float far);
    void setClearColor(const MTL::ClearColor &color) { clearColor = color; }

private:
    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    CA::MetalDrawable *drawableArea;
    MTL::CommandQueue *commandQueue;
    MTL::DepthStencilState *depthState;
    MTL::DepthStencilState *depthStateUI;
    MTL::Texture *depthTexture;

    std::vector<std::shared_ptr<Renderable>> renderables;
    MTL::ClearColor clearColor;
    float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
};

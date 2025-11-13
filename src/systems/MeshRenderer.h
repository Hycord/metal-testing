#pragma once
#include "config.h"
#include "components/engine/Renderable.h"
#include "components/renderables/core/UIContainer.h"
#include <vector>


enum class ProjectionMode {
    Perspective,
    Orthographic,
    Custom
};

class MeshRenderer {
public:
    MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer);
    ~MeshRenderer();

    void addRenderable(Renderable *r);
    void draw(const simd::float4x4 &view);
    void draw(const simd::float4x4 &view, const std::vector<std::shared_ptr<UIContainer>> &uiElements);
    void drawUI(const std::vector<std::shared_ptr<UIContainer>> &uiElements, MTL::RenderCommandEncoder *encoder);

    MTL::CommandBuffer* createUICommandBuffer();
    MTL::RenderCommandEncoder* createUIEncoder(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable** outDrawable);
    void commitUICommandBuffer(MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* encoder, CA::MetalDrawable* drawable);

    void setProjectionMode(ProjectionMode mode) { projectionMode = mode; }
    void setCustomProjection(const simd::float4x4 &m) { customProjection = m; projectionMode = ProjectionMode::Custom; }
    void setOrthoParams(float left, float right, float bottom, float top, float near, float far);

private:
    simd::float4x4 computeProjection();

    MTL::Device *device;
    CA::MetalLayer *metalLayer;
    CA::MetalDrawable *drawableArea;
    MTL::CommandQueue *commandQueue;
    MTL::DepthStencilState *depthState;
    MTL::DepthStencilState *depthStateUI;
    MTL::Texture *depthTexture;

    std::vector<Renderable*> renderables;
    ProjectionMode projectionMode;
    simd::float4x4 customProjection;
    float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
};

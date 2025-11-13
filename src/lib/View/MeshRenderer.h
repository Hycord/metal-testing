#pragma once
#include "../../config.h"
#include "Renderable.h"
#include "UIContainer.h"
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
    // Draw scene and UI in a single render pass/encoder to avoid presenting multiple drawables.
    void draw(const simd::float4x4 &view, const std::vector<std::shared_ptr<UIContainer>> &uiElements);
    void drawUI(const std::vector<std::shared_ptr<UIContainer>> &uiElements, MTL::RenderCommandEncoder *encoder); // draw UI into an already-created encoder

    // Helper APIs to create a standalone command buffer + encoder for UI rendering so UI
    // can be updated/committed independently from the main draw pass.
    // - createUICommandBuffer: returns a new command buffer from the renderer's queue (retain returned pointer)
    // - createUIEncoder: given a command buffer, obtains a drawable and returns a render encoder that targets it.
    //   The drawable is returned via the outDrawable pointer and must be used when committing/presenting.
    // - commitUICommandBuffer: ends the encoder, presents the drawable and commits the command buffer.
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
    MTL::DepthStencilState *depthStateUI; // depth disabled for UI overlays
    MTL::Texture *depthTexture;

    std::vector<Renderable*> renderables;
    ProjectionMode projectionMode;
    simd::float4x4 customProjection;
    // orthographic params
    float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
};

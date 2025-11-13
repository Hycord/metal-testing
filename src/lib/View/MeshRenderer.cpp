#include "MeshRenderer.h"
#include "../LogManager/LogManager.h"
#include "PipelineFactory.h"
#include "MeshFactory.h"
#include <cmath>
#include "../Utils/Math.h"
#include "UIElement.h"

MeshRenderer::MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer)
    : device(device->retain()), metalLayer(metalLayer->retain()), commandQueue(device->newCommandQueue()->retain()), projectionMode(ProjectionMode::Perspective)
{
    LOG_CONSTRUCT("MeshRenderer");
    customProjection = MetalMath::identity();
    orthoLeft = -1.0f;
    orthoRight = 1.0f;
    orthoBottom = -1.0f;
    orthoTop = 1.0f;
    orthoNear = -1.0f;
    orthoFar = 1.0f;
    depthState = nullptr;
    depthTexture = nullptr;

    // create a default depth-stencil state with depth writes enabled and a LESS compare function
    MTL::DepthStencilDescriptor *dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
    dsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    dsDesc->setDepthWriteEnabled(true);
    depthState = device->newDepthStencilState(dsDesc);
    dsDesc->release();
}

MeshRenderer::~MeshRenderer()
{
    LOG_DESTROY("MeshRenderer");
    for (auto *r : renderables)
        delete r;
    commandQueue->release();
    metalLayer->release();
    device->release();
    if (depthState)
        depthState->release();
    if (depthTexture)
        depthTexture->release();
}

void MeshRenderer::addRenderable(Renderable *r)
{
    renderables.push_back(r);
}

void MeshRenderer::setOrthoParams(float left, float right, float bottom, float top, float near, float far)
{
    orthoLeft = left;
    orthoRight = right;
    orthoBottom = bottom;
    orthoTop = top;
    orthoNear = near;
    orthoFar = far;
    projectionMode = ProjectionMode::Orthographic;
}

simd::float4x4 MeshRenderer::computeProjection()
{
    if (projectionMode == ProjectionMode::Custom)
        return customProjection;
    if (projectionMode == ProjectionMode::Perspective)
    {
        // simple fixed aspect and params; applications can set custom by setCustomProjection
        return MetalMath::perspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    }

    // Orthographic matrix
    float l = orthoLeft, r = orthoRight, b = orthoBottom, t = orthoTop, n = orthoNear, f = orthoFar;
    // column-major simd_matrix
    simd_float4 col0 = {2.0f / (r - l), 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, 2.0f / (t - b), 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f / (f - n), 0.0f};
    simd_float4 col3 = {-(r + l) / (r - l), -(t + b) / (t - b), -n / (f - n), 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

// Always compute orthographic projection from current ortho params (left/right/top/bottom set via setOrthoParams)
simd::float4x4 computeOrtho(float l, float r, float b, float t, float n, float f)
{
    simd_float4 col0 = {2.0f / (r - l), 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, 2.0f / (t - b), 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f / (f - n), 0.0f};
    simd_float4 col3 = {-(r + l) / (r - l), -(t + b) / (t - b), -n / (f - n), 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

void MeshRenderer::draw(const simd::float4x4 &view)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();

    drawableArea = metalLayer->nextDrawable();
    if (!drawableArea)
    {
        LOG_ERROR("MeshRenderer::draw: metalLayer->nextDrawable() returned null - skipping frame");
        pool->release();
        return;
    }
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(drawableArea->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    // Ensure we have a depth texture of the same size as the drawable. Recreate if size changed.
    uint32_t drawableWidth = drawableArea->texture()->width();
    uint32_t drawableHeight = drawableArea->texture()->height();
    bool needNewDepth = false;
    if (!depthTexture)
        needNewDepth = true;
    else if (depthTexture->width() != drawableWidth || depthTexture->height() != drawableHeight)
        needNewDepth = true;
    if (needNewDepth)
    {
        if (depthTexture)
        {
            depthTexture->release();
            depthTexture = nullptr;
        }
        MTL::TextureDescriptor *desc = MTL::TextureDescriptor::texture2DDescriptor(
            MTL::PixelFormat::PixelFormatDepth32Float,
            drawableWidth,
            drawableHeight,
            false);
        desc->setUsage(MTL::TextureUsageRenderTarget);
        desc->setStorageMode(MTL::StorageModePrivate);
        depthTexture = device->newTexture(desc);
    }

    // attach depth texture to the render pass so depth testing can occur
    MTL::RenderPassDepthAttachmentDescriptor *depthAttachment = renderPass->depthAttachment();
    depthAttachment->setTexture(depthTexture);
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setClearDepth(1.0f);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);

    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    
    // set the depth-stencil state so the GPU performs depth testing/writes
    if (depthState)
    {
        encoder->setDepthStencilState(depthState);
    }

    simd::float4x4 projection = computeProjection();

    // compute both perspective and orthographic (for screen-space overlays)
    simd::float4x4 perspective = MetalMath::perspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    simd::float4x4 ortho = computeOrtho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);

    for (auto *r : renderables)
    {
        if (r->isScreenSpace())
        {
            // for screen-space draw, use ortho and an identity view
            r->draw(encoder, ortho, MetalMath::identity());
        }
        else
        {
            r->draw(encoder, perspective, view);
        }
    }

    encoder->endEncoding();
    commandBuffer->presentDrawable(drawableArea);
    commandBuffer->commit();

    pool->release();
}

void MeshRenderer::draw(const simd::float4x4 &view, const std::vector<std::shared_ptr<UIContainer>> &uiElements)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    auto frameStart = std::chrono::high_resolution_clock::now();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();

    drawableArea = metalLayer->nextDrawable();
    if (!drawableArea)
    {
        LOG_ERROR("MeshRenderer::draw(scene+UI): metalLayer->nextDrawable() returned null - skipping frame");
        pool->release();
        return;
    }
    auto afterDrawable = std::chrono::high_resolution_clock::now();
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(drawableArea->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    // Ensure depth texture exists and is sized correctly
    uint32_t drawableWidth = drawableArea->texture()->width();
    uint32_t drawableHeight = drawableArea->texture()->height();
    bool needNewDepth = false;
    if (!depthTexture)
        needNewDepth = true;
    else if (depthTexture->width() != drawableWidth || depthTexture->height() != drawableHeight)
        needNewDepth = true;
    std::chrono::high_resolution_clock::time_point afterDepthAlloc = afterDrawable;
    if (needNewDepth)
    {
        if (depthTexture)
        {
            depthTexture->release();
            depthTexture = nullptr;
        }
        MTL::TextureDescriptor *desc = MTL::TextureDescriptor::texture2DDescriptor(
            MTL::PixelFormat::PixelFormatDepth32Float,
            drawableWidth,
            drawableHeight,
            false);
        desc->setUsage(MTL::TextureUsageRenderTarget);
        desc->setStorageMode(MTL::StorageModePrivate);
        depthTexture = device->newTexture(desc);
        afterDepthAlloc = std::chrono::high_resolution_clock::now();
    }

    MTL::RenderPassDepthAttachmentDescriptor *depthAttachment = renderPass->depthAttachment();
    depthAttachment->setTexture(depthTexture);
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setClearDepth(1.0f);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);

    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);
    auto afterEncoder = std::chrono::high_resolution_clock::now();

    if (depthState)
    {
        encoder->setDepthStencilState(depthState);
    }

    simd::float4x4 projection = computeProjection();

    simd::float4x4 perspective = MetalMath::perspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    simd::float4x4 ortho = computeOrtho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);

    auto beforeScene = std::chrono::high_resolution_clock::now();
    for (auto *r : renderables)
    {
        if (r->isScreenSpace())
        {
            r->draw(encoder, ortho, MetalMath::identity());
        }
        else
        {
            r->draw(encoder, perspective, view);
        }
    }
    auto afterScene = std::chrono::high_resolution_clock::now();

    // If any UI elements implement UIElement::setScreenSize, update them with the drawable size
    for (const auto &uiElement : uiElements)
    {
        if (!uiElement) continue;
        UIElement* uiel = dynamic_cast<UIElement*>(uiElement.get());
        if (uiel) {
            uiel->setScreenSize(float(drawableWidth), float(drawableHeight));
        }
    }
    auto beforeUI = std::chrono::high_resolution_clock::now();

    // Draw UI elements into the same encoder so we only present once and avoid races/crashes
    for (const auto &uiElement : uiElements)
    {
        if (uiElement)
        {
            uiElement->render(encoder);
        }
    }
    auto afterUI = std::chrono::high_resolution_clock::now();

    encoder->endEncoding();
    auto afterEndEncoding = std::chrono::high_resolution_clock::now();
    commandBuffer->presentDrawable(drawableArea);
    auto afterPresent = std::chrono::high_resolution_clock::now();
    commandBuffer->commit();
    auto afterCommit = std::chrono::high_resolution_clock::now();

    // Log timings for this frame to help identify bottlenecks
    auto frameEnd = std::chrono::high_resolution_clock::now();
    using ms = std::chrono::duration<double, std::milli>;
    double t_total = ms(frameEnd - frameStart).count();
    double t_acquire = ms(afterDrawable - frameStart).count();
    double t_depth = (needNewDepth ? ms(afterDepthAlloc - afterDrawable).count() : 0.0);
    double t_encoder = ms(afterEncoder - afterDrawable).count();
    double t_scene = ms(afterScene - beforeScene).count();
    double t_ui = ms(afterUI - beforeUI).count();
    double t_end = ms(afterEndEncoding - afterUI).count();
    double t_present = ms(afterPresent - afterEndEncoding).count();
    double t_commit = ms(afterCommit - afterPresent).count();

    LOG_DEBUG("Frame timings ms: total=%.2f acquire=%.2f depthAlloc=%.2f encoderSetup=%.2f scene=%.2f ui=%.2f endEncode=%.2f present=%.2f commit=%.2f",
              t_total, t_acquire, t_depth, t_encoder, t_scene, t_ui, t_end, t_present, t_commit);

    pool->release();
}

void MeshRenderer::drawUI(const std::vector<std::shared_ptr<UIContainer>> &uiElements, MTL::RenderCommandEncoder *encoder)
{
    for (const auto &uiElement : uiElements)
    {
        if (uiElement)
        {
            uiElement->render(encoder);
        }
    }
}

MTL::CommandBuffer* MeshRenderer::createUICommandBuffer()
{
    // Return a new command buffer from the renderer's queue. Caller is responsible for committing it.
    if (!commandQueue)
        return nullptr;
    return commandQueue->commandBuffer();
}

MTL::RenderCommandEncoder* MeshRenderer::createUIEncoder(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable** outDrawable)
{
    if (!commandBuffer || !metalLayer)
    {
        if (outDrawable)
            *outDrawable = nullptr;
        return nullptr;
    }

    MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();

    CA::MetalDrawable *uiDrawable = metalLayer->nextDrawable();
    if (!uiDrawable)
    {
        LOG_ERROR("MeshRenderer::createUIEncoder: metalLayer->nextDrawable() returned null - cannot create UI encoder");
        if (outDrawable)
            *outDrawable = nullptr;
        renderPass->release();
        return nullptr;
    }

    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(uiDrawable->texture());
    // We want to draw the UI on top of whatever is already in the drawable if present.
    colorAttachment->setLoadAction(MTL::LoadActionLoad);
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    // No depth for UI overlays by default

    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    if (outDrawable)
        *outDrawable = uiDrawable;

    // Note: renderPass intentionally not released here to match style in draw(); caller/encoder uses it transiently
    return encoder;
}

void MeshRenderer::commitUICommandBuffer(MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* encoder, CA::MetalDrawable* drawable)
{
    if (!commandBuffer || !encoder || !drawable)
        return;

    encoder->endEncoding();
    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit();
}

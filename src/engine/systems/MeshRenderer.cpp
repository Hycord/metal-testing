#include "engine/systems/MeshRenderer.h"
#include "engine/core/LogManager.h"
#include "engine/utils/Math.h"
#include "engine/components/renderables/core/UIElement.h"
#include "engine/systems/input/InputState.h"

#include <chrono>
#include <cmath>

MeshRenderer::MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer)
    : device(device->retain()),
      metalLayer(metalLayer->retain()),
      drawableArea(nullptr),
      commandQueue(device->newCommandQueue()->retain()),
      depthState(nullptr),
      depthStateUI(nullptr),
      depthTexture(nullptr),
      clearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0))
{
    LOG_CONSTRUCT("MeshRenderer");
    orthoLeft = -1.0f;
    orthoRight = 1.0f;
    orthoBottom = -1.0f;
    orthoTop = 1.0f;
    orthoNear = -1.0f;
    orthoFar = 1.0f;

    
    MTL::DepthStencilDescriptor *dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
    dsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    dsDesc->setDepthWriteEnabled(true);
    depthState = device->newDepthStencilState(dsDesc);
    dsDesc->release();

    
    MTL::DepthStencilDescriptor *dsUIDesc = MTL::DepthStencilDescriptor::alloc()->init();
    dsUIDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionAlways);
    dsUIDesc->setDepthWriteEnabled(false);
    depthStateUI = device->newDepthStencilState(dsUIDesc);
    dsUIDesc->release();
}

MeshRenderer::~MeshRenderer()
{
    LOG_DESTROY("MeshRenderer");
    renderables.clear();
    commandQueue->release();
    metalLayer->release();
    device->release();
    if (depthState)
        depthState->release();
    if (depthStateUI)
        depthStateUI->release();
    if (depthTexture)
        depthTexture->release();
}

void MeshRenderer::addRenderable(const std::shared_ptr<Renderable> &r)
{
    renderables.push_back(r);
}

void MeshRenderer::clearRenderables()
{
    renderables.clear();
}

void MeshRenderer::setOrthoParams(float left, float right, float bottom, float top, float near, float far)
{
    orthoLeft = left;
    orthoRight = right;
    orthoBottom = bottom;
    orthoTop = top;
    orthoNear = near;
    orthoFar = far;
}
void MeshRenderer::draw(const CameraMatrices &camera, const std::vector<std::shared_ptr<UIContainer>> &uiElements, const std::vector<std::shared_ptr<WorldContainer>> &worldElements)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    auto frameStart = std::chrono::high_resolution_clock::now();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();

    drawableArea = metalLayer->nextDrawable();
    if (!drawableArea)
    {
        LOG_ERROR("MeshRenderer::draw: metalLayer->nextDrawable() returned null - skipping frame");
        pool->release();
        renderPass->release();
        return;
    }
    auto afterDrawable = std::chrono::high_resolution_clock::now();
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(drawableArea->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(clearColor);
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    
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

    const simd::float4x4 worldProjection = camera.projection;
    const simd::float4x4 worldView = camera.view;
    const simd::float4x4 ortho = MetalMath::orthographicProjection(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
    const simd::float4x4 identity = MetalMath::identity();

    auto beforeScene = std::chrono::high_resolution_clock::now();
    for (const auto &renderable : renderables)
    {
        if (!renderable)
        {
            continue;
        }

        if (renderable->isScreenSpace())
        {
            if (depthStateUI)
            {
                encoder->setDepthStencilState(depthStateUI);
            }
            renderable->draw(encoder, ortho, identity);
            if (depthState)
            {
                encoder->setDepthStencilState(depthState);
            }
        }
        else
        {
            renderable->draw(encoder, worldProjection, worldView);
        }
    }
    auto afterScene = std::chrono::high_resolution_clock::now();

    for (const auto &worldElement : worldElements)
    {
        if (worldElement)
        {
            worldElement->render(encoder, worldProjection, worldView);
        }
    }
    auto afterWorld = std::chrono::high_resolution_clock::now();

    auto beforeUI = std::chrono::high_resolution_clock::now();

    
    if (depthStateUI) {
        encoder->setDepthStencilState(depthStateUI);
    }
    
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

    renderPass->release();
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
    
    colorAttachment->setLoadAction(MTL::LoadActionLoad);
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    

    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    if (outDrawable)
        *outDrawable = uiDrawable;

    
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

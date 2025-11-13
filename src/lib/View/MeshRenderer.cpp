#include "MeshRenderer.h"
#include "../LogManager/LogManager.h"
#include "PipelineFactory.h"
#include "MeshFactory.h"
#include <cmath>
#include "../Utils/Math.h"

MeshRenderer::MeshRenderer(MTL::Device *device, CA::MetalLayer *metalLayer)
    : device(device->retain()), metalLayer(metalLayer->retain()), commandQueue(device->newCommandQueue()->retain()), projectionMode(ProjectionMode::Perspective)
{
    LOG_CONSTRUCT("MeshRenderer");
    customProjection = MetalMath::identity();
    orthoLeft = -1.0f; orthoRight = 1.0f; orthoBottom = -1.0f; orthoTop = 1.0f; orthoNear = -1.0f; orthoFar = 1.0f;
}

MeshRenderer::~MeshRenderer()
{
    LOG_DESTROY("MeshRenderer");
    for (auto *r : renderables) delete r;
    commandQueue->release();
    metalLayer->release();
    device->release();
}

void MeshRenderer::addRenderable(Renderable *r)
{
    renderables.push_back(r);
}

void MeshRenderer::setOrthoParams(float left, float right, float bottom, float top, float near, float far)
{
    orthoLeft = left; orthoRight = right; orthoBottom = bottom; orthoTop = top; orthoNear = near; orthoFar = far;
    projectionMode = ProjectionMode::Orthographic;
}

simd::float4x4 MeshRenderer::computeProjection()
{
    if (projectionMode == ProjectionMode::Custom) return customProjection;
    if (projectionMode == ProjectionMode::Perspective) {
        // simple fixed aspect and params; applications can set custom by setCustomProjection
        return MetalMath::perspectiveProjection(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
    }

    // Orthographic matrix
    float l = orthoLeft, r = orthoRight, b = orthoBottom, t = orthoTop, n = orthoNear, f = orthoFar;
    // column-major simd_matrix
    simd_float4 col0 = {2.0f/(r-l), 0.0f, 0.0f, 0.0f};
    simd_float4 col1 = {0.0f, 2.0f/(t-b), 0.0f, 0.0f};
    simd_float4 col2 = {0.0f, 0.0f, 1.0f/(f-n), 0.0f};
    simd_float4 col3 = {-(r+l)/(r-l), -(t+b)/(t-b), -n/(f-n), 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

void MeshRenderer::draw(const simd::float4x4 &view)
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor *renderPass = MTL::RenderPassDescriptor::alloc()->init();

    drawableArea = metalLayer->nextDrawable();
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPass->colorAttachments()->object(0);
    colorAttachment->setTexture(drawableArea->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPass);

    simd::float4x4 projection = computeProjection();

    for (auto *r : renderables) {
        r->draw(encoder, projection, view);
    }

    encoder->endEncoding();
    commandBuffer->presentDrawable(drawableArea);
    commandBuffer->commit();

    pool->release();
}

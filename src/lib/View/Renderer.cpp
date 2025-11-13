#include "Renderer.h"
#include "../LogManager/LogManager.h"
#include "../Utils/FileReader.h"
#include "PipelineFactory.h"
#include "../Utils/Math.h"
#include <cstddef>

Renderer::Renderer(MTL::Device *device, CA::MetalLayer *metalLayer) : device(device->retain()),
                                                                      metalLayer(metalLayer->retain()),
                                                                      commandQueue(device->newCommandQueue()->retain())
{
    LOG_CONSTRUCT("Renderer");
    buildMeshes();
    buildShaders();
}

Renderer::~Renderer()
{
    LOG_DESTROY("Renderer");
    quadMesh.indexBuffer->release();
    quadMesh.vertexBuffer->release();
    if (quadMesh.vertexDescriptor) {
        quadMesh.vertexDescriptor->release();
    }

    triangleMesh->release();
    trianglePipeline->release();
    generalPipeline->release();
    commandQueue->release();
    device->release();
}

void Renderer::buildMeshes()
{
    triangleMesh = MeshFactory::buildTriangle(device);
    quadMesh = MeshFactory::buildQuad(device);
}

void Renderer::buildShaders()
{
    LOG_START("Renderer: Reloading Shader Files");
    LOG_DEBUG("Renderer creating PipelineFactory");
    PipelineFactory *builder = new PipelineFactory(device);
    LOG_DEBUG("Renderer PipelineFactory created: %p", builder);

    LOG_DEBUG("Renderer set vertex descriptor");
    builder->set_vertex_descriptor(quadMesh.vertexDescriptor);
    builder->set_filename("Triangle");
    builder->set_vertex_entry_point("vertexTriangle");
    builder->set_fragment_entry_point("fragmentTriangle");
    LOG_DEBUG("Renderer building triangle pipeline");
    trianglePipeline = builder->build();
    LOG_DEBUG("Renderer trianglePipeline=%p", trianglePipeline);

    builder->set_filename("General");
    builder->set_vertex_entry_point("vertexGeneral");
    builder->set_fragment_entry_point("fragmentGeneral");
    LOG_DEBUG("Renderer building general pipeline");
    generalPipeline = builder->build();
    LOG_DEBUG("Renderer generalPipeline=%p", generalPipeline);

    delete builder;
    LOG_FINISH("Renderer: shader loading finished");
}

void Renderer::draw(const simd::float4x4 &view)
{

    t += 1.0f;
    if (t > 360)
    {
        t -= 360.0f;
    }

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

    encoder->setRenderPipelineState(generalPipeline);

    simd::float4x4 projection = MetalMath::perspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    simd::float4x4 transform = MetalMath::translate({0.0f, 0.0f, 3.0f});

    encoder->setVertexBytes(&projection, sizeof(simd::float4x4), 2);
    encoder->setVertexBytes(&view, sizeof(simd::float4x4), 3);
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);

    encoder->setVertexBuffer(quadMesh.vertexBuffer, 0, 0);
    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(6), MTL::IndexType::IndexTypeUInt16, quadMesh.indexBuffer, NS::UInteger(0), NS::UInteger(6));

    transform = MetalMath::translate({0.5f, 0.5f, 2.0f}) * MetalMath::rotateZ(t) * MetalMath::scale(0.1f);
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);

    encoder->setVertexBuffer(triangleMesh, 0, 0);
    encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    encoder->endEncoding();
    commandBuffer->presentDrawable(drawableArea);
    commandBuffer->commit();

    pool->release();
}
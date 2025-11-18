#include "engine/components/engine/Renderable.h"
#include "engine/core/LogManager.h"
#include "engine/utils/Math.h"

Renderable::Renderable(const Mesh &m, Material *mat) : mesh(m), material(mat), transform(MetalMath::identity()) {
    screenSpace = false;
    
    if (mesh.vertexBuffer) mesh.vertexBuffer->retain();
    if (mesh.indexBuffer) mesh.indexBuffer->retain();
    if (mesh.vertexDescriptor) mesh.vertexDescriptor->retain();
}

Renderable::~Renderable()
{
    LOG_DESTROY("Renderable");
    if (mesh.vertexBuffer) mesh.vertexBuffer->release();
    if (mesh.indexBuffer) mesh.indexBuffer->release();
    if (mesh.vertexDescriptor) mesh.vertexDescriptor->release();
    if (material) delete material;
}

void Renderable::draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &projection, const simd::float4x4 &view)
{
    if (!material) return;

    material->apply(encoder);

    encoder->setDepthBias(depthBias, depthBiasSlopeScale, 0.0f);
    LOG_DEBUG("Renderable::draw depthBias=%.4f slopeScale=%.4f", depthBias, depthBiasSlopeScale);

    
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);
    encoder->setVertexBytes(&projection, sizeof(simd::float4x4), 2);
    encoder->setVertexBytes(&view, sizeof(simd::float4x4), 3);

    if (mesh.vertexBuffer) {
        encoder->setVertexBuffer(mesh.vertexBuffer, 0, 0);
    }

    if (mesh.indexBuffer) {
        
        encoder->drawIndexedPrimitives(primitiveType,
                                       NS::UInteger(mesh.indexCount),
                                       MTL::IndexType::IndexTypeUInt16,
                                       mesh.indexBuffer,
                                       NS::UInteger(0),
                                       NS::UInteger(1));
    } else if (mesh.vertexBuffer) {
        
        encoder->drawPrimitives(primitiveType, NS::UInteger(0), NS::UInteger(mesh.vertexCount));
    }
}

void Renderable::updateMesh(const Mesh &m)
{
    if (mesh.vertexBuffer) {
        mesh.vertexBuffer->release();
        mesh.vertexBuffer = nullptr;
    }
    if (mesh.indexBuffer) {
        mesh.indexBuffer->release();
        mesh.indexBuffer = nullptr;
    }
    if (mesh.vertexDescriptor) {
        mesh.vertexDescriptor->release();
        mesh.vertexDescriptor = nullptr;
    }

    mesh = m;

    if (mesh.vertexBuffer) mesh.vertexBuffer->retain();
    if (mesh.indexBuffer) mesh.indexBuffer->retain();
    if (mesh.vertexDescriptor) mesh.vertexDescriptor->retain();
}

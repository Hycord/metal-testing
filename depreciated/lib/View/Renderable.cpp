#include "Renderable.h"
#include "../LogManager/LogManager.h"
#include "../Utils/Math.h"

Renderable::Renderable(const Mesh &m, Material *mat) : mesh(m), material(mat), transform(MetalMath::identity()) {
    screenSpace = false;
    // retain buffers so lifetime is explicit (match project's retain/release style where needed)
    if (mesh.vertexBuffer) mesh.vertexBuffer->retain();
    if (mesh.indexBuffer) mesh.indexBuffer->retain();
    if (mesh.vertexDescriptor) mesh.vertexDescriptor->retain();
}

Renderable::~Renderable()
{
    if (mesh.vertexBuffer) mesh.vertexBuffer->release();
    if (mesh.indexBuffer) mesh.indexBuffer->release();
    if (mesh.vertexDescriptor) mesh.vertexDescriptor->release();
    if (material) delete material;
}

void Renderable::draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &projection, const simd::float4x4 &view)
{
    if (!material) return;

    material->apply(encoder);

    // set transform, projection and view at the indices expected by existing shaders
    encoder->setVertexBytes(&transform, sizeof(simd::float4x4), 1);
    encoder->setVertexBytes(&projection, sizeof(simd::float4x4), 2);
    encoder->setVertexBytes(&view, sizeof(simd::float4x4), 3);

    if (mesh.vertexBuffer) {
        encoder->setVertexBuffer(mesh.vertexBuffer, 0, 0);
    }

    if (mesh.indexBuffer) {
        // indexed draw
        encoder->drawIndexedPrimitives(primitiveType,
                                       NS::UInteger(mesh.indexCount),
                                       MTL::IndexType::IndexTypeUInt16,
                                       mesh.indexBuffer,
                                       NS::UInteger(0),
                                       NS::UInteger(1));
    } else if (mesh.vertexBuffer) {
        // non-indexed; assume 3 vertices as in triangle
        encoder->drawPrimitives(primitiveType, NS::UInteger(0), NS::UInteger(mesh.vertexCount));
    }
}

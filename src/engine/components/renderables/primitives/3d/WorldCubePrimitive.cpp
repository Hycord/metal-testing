#include "engine/components/renderables/primitives/3d/WorldCubePrimitive.h"
#include "engine/factories/MeshFactory.h"
#include "engine/core/LogManager.h"

WorldCubePrimitive::WorldCubePrimitive(MTL::Device *device, float s, const simd::float4 &col, const simd::float3 &position)
    : device(device), size(s), position(position)
{
    LOG_CONSTRUCT("WorldCubePrimitive");
    setColor(col);
    setScreenSpace(false);
    setPrimitiveType(MTL::PrimitiveType::PrimitiveTypeTriangle);
    rebuild();
    updateTransform();
}

WorldCubePrimitive::~WorldCubePrimitive()
{
    LOG_DESTROY("WorldCubePrimitive");
}

void WorldCubePrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    LOG_DEBUG("WorldCubePrimitive: rebuilding mesh");

    float halfSize = size * 0.5f;
    
    Vertex vertices[8] = {
        {{-halfSize, -halfSize, -halfSize}, {color.x, color.y, color.z}, {0.0f, 0.0f}},
        {{halfSize, -halfSize, -halfSize},  {color.x, color.y, color.z}, {1.0f, 0.0f}},
        {{halfSize, halfSize, -halfSize},   {color.x, color.y, color.z}, {1.0f, 1.0f}},
        {{-halfSize, halfSize, -halfSize},  {color.x, color.y, color.z}, {0.0f, 1.0f}},
        {{-halfSize, -halfSize, halfSize},  {color.x, color.y, color.z}, {0.0f, 0.0f}},
        {{halfSize, -halfSize, halfSize},   {color.x, color.y, color.z}, {1.0f, 0.0f}},
        {{halfSize, halfSize, halfSize},    {color.x, color.y, color.z}, {1.0f, 1.0f}},
        {{-halfSize, halfSize, halfSize},   {color.x, color.y, color.z}, {0.0f, 1.0f}}
    };

    ushort indices[36] = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 6, 4, 7,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3
    };

    mesh.vertexBuffer = device->newBuffer(8 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices, 8 * sizeof(Vertex));

    mesh.indexBuffer = device->newBuffer(36 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 36 * sizeof(ushort));

    mesh.vertexCount = 8;
    mesh.indexCount = 36;

    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();
    
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    positionDescriptor->setOffset(offsetof(Vertex, position));
    positionDescriptor->setBufferIndex(0);
    
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setBufferIndex(0);
    colorDescriptor->setOffset(offsetof(Vertex, color));
    
    auto uvDescriptor = attributes->object(2);
    uvDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
    uvDescriptor->setBufferIndex(0);
    uvDescriptor->setOffset(offsetof(Vertex, uv));
    
    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(sizeof(Vertex));
    
    mesh.vertexDescriptor = vertexDescriptor;

    if (!renderable) {
        renderable = makeRenderable(device, mesh, color, getPrimitiveType());
    } else {
        renderable->updateMesh(mesh);
        if (auto material = renderable->getMaterial()) {
            material->setColor(color);
        }
    }

    dirty = false;
}

void WorldCubePrimitive::draw(MTL::RenderCommandEncoder *encoder,
                         const simd::float4x4 &projection,
                         const simd::float4x4 &view)
{
    rebuild();
    if (renderable)
        renderable->draw(encoder, projection, view);
}

void WorldCubePrimitive::onColorChanged()
{
    dirty = true;
}

void WorldCubePrimitive::onTransformChanged()
{
    if (renderable) {
        LOG_DEBUG("WorldCubePrimitive: transform changed");
    }
}

void WorldCubePrimitive::updateTransform()
{
    simd::float4x4 translationMatrix = MetalMath::translate(position.x, position.y, position.z);
    if (renderable) {
        renderable->setTransform(translationMatrix);
    }
    LOG_DEBUG("WorldCubePrimitive: updated transform at ({}, {}, {})", position.x, position.y, position.z);
}

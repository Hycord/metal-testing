#include "components/renderables/primitives/UIPrimitiveCircle.h"
#include "utils/Math.h"
#include "systems/input/InputState.h"
#include "factories/CircleFactory.h"

// CircleUIPrimitive implementation
CircleUIPrimitive::CircleUIPrimitive(MTL::Device *device, float x, float y, float r, const simd::float4 &col, int segs)
    : device(device), cx(x), cy(y), radius(r), segments(segs)
{
    color = col;
    // We'll emit explicit triangle indices; use triangle list primitive
    primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
}

void CircleUIPrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    // Triangle fan (implemented as triangle list via indices):
    // center + one vertex per segment + repeat first segment vertex
    int vertCount = segments + 2;
    std::vector<Vertex> vertices(vertCount);
    std::vector<ushort> indices((segments + 1) * 3);

    // Center vertex
    vertices[0] = {{cx, cy, 0.0f}, {color.x, color.y, color.z}};

    // Perimeter vertices via CircleFactory
    CircleFactory circle(radius, segments);
    for (int i = 0; i <= segments; ++i)
    {
        simd::float2 p = circle.getPoint(i);
        float x = cx + p.x;
        float y = cy + p.y;
        vertices[i + 1] = {{x, y, 0.0f}, {color.x, color.y, color.z}};
    }

    // indices for triangle fan
    for (int i = 0; i < segments; ++i)
    {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = static_cast<ushort>(i + 1);
        indices[i * 3 + 2] = static_cast<ushort>(i + 2);
    }

    // Create buffers
    mesh.vertexBuffer = device->newBuffer(vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices.data(), vertices.size() * sizeof(Vertex));
    mesh.indexBuffer = device->newBuffer(indices.size() * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices.data(), indices.size() * sizeof(ushort));
    mesh.vertexCount = vertices.size();
    mesh.indexCount = indices.size();

    // Vertex descriptor
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
    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(sizeof(Vertex));
    mesh.vertexDescriptor = vertexDescriptor;

    // Build renderable
    renderable = makeRenderable(device, mesh, color, primitiveType);
    dirty = false;
}

void CircleUIPrimitive::draw(MTL::RenderCommandEncoder *encoder, const simd::float4x4 &ortho)
{
    rebuild();
    if (!renderable)
        return;
    renderable->draw(encoder, ortho, MetalMath::identity());
}

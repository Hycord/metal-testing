#include "engine/components/renderables/primitives/2d/CirclePrimitive.h"
#include "engine/utils/Math.h"
#include "engine/systems/input/InputState.h"
#include "engine/factories/CircleFactory.h"


CirclePrimitive::CirclePrimitive(MTL::Device *device, float x, float y, float r, const simd::float4 &col, int segs)
    : device(device), cx(x), cy(y), radius(r), segments(segs)
{
    setColor(col);
    
    setPrimitiveType(MTL::PrimitiveType::PrimitiveTypeTriangle);
}

void CirclePrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    
    
    int vertCount = segments + 2;
    std::vector<Vertex> vertices(vertCount);
    std::vector<ushort> indices((segments + 1) * 3);

    
    vertices[0] = {{cx, cy, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};

    
    CircleFactory circle(radius, segments);
    for (int i = 0; i <= segments; ++i)
    {
        simd::float2 p = circle.getPoint(i);
        float x = cx + p.x;
        float y = cy + p.y;
        vertices[i + 1] = {{x, y, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};
    }

    
    for (int i = 0; i < segments; ++i)
    {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = static_cast<ushort>(i + 1);
        indices[i * 3 + 2] = static_cast<ushort>(i + 2);
    }

    
    mesh.vertexBuffer = device->newBuffer(vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices.data(), vertices.size() * sizeof(Vertex));
    mesh.indexBuffer = device->newBuffer(indices.size() * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices.data(), indices.size() * sizeof(ushort));
    mesh.vertexCount = vertices.size();
    mesh.indexCount = indices.size();

    
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

void CirclePrimitive::draw(MTL::RenderCommandEncoder *encoder,
                             const simd::float4x4 &projection,
                             const simd::float4x4 &view)
{
    rebuild();
    if (!renderable)
        return;
    renderable->draw(encoder, projection, view);
}

void CirclePrimitive::onColorChanged()
{
    if (renderable)
    {
        if (auto material = renderable->getMaterial())
        {
            material->setColor(getColor());
        }
    }
}

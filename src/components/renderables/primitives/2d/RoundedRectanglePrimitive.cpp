#include "components/renderables/primitives/2d/RoundedRectanglePrimitive.h"
#include "utils/Math.h"
#include "systems/input/InputState.h"
#include "factories/RoundedRectangleFactory.h"

RoundedRectanglePrimitive::RoundedRectanglePrimitive(MTL::Device* device,
                                                         float left, float top,
                                                         float w, float h,
                                                         float radius,
                                                         const simd::float4 &col,
                                                         int qualityPerCorner)
    : device(device), l(left), t(top), width(w), height(h), rad(radius), perCorner(qualityPerCorner < 1 ? 1 : qualityPerCorner)
{
    setColor(col);
    
    setPrimitiveType(MTL::PrimitiveType::PrimitiveTypeTriangle);
}

void RoundedRectanglePrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    
    float maxRad = 0.5f * std::min(width, height);
    float r = std::max(0.0f, std::min(rad, maxRad));

    
    RoundedRectangleFactory poly(simd::float2{width, height}, r, perCorner);
    int N = poly.pointCount(); 

    
    std::vector<Vertex> vertices(1 + N + 1);

    
    float cx = l + width * 0.5f;
    float cy = t + height * 0.5f;
    vertices[0] = {{cx, cy, 0.0f}, {color.x, color.y, color.z}, {0.5f, 0.5f}};

    
    for (int i = 0; i <= N; ++i) {
        int idx = (i == N) ? 0 : i;
        simd::float2 p = poly.getPoint(idx);
        float x = l + p.x;
        float y = t + p.y;
        float u = width > 0.0f ? (x - l) / width : 0.0f;
        float v = height > 0.0f ? 1.0f - (y - t) / height : 0.0f;
        vertices[i + 1] = {{x, y, 0.0f}, {color.x, color.y, color.z}, {u, v}};
    }

    
    std::vector<ushort> indices(N * 3);
    for (int i = 0; i < N; ++i) {
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

void RoundedRectanglePrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                       const simd::float4x4 &projection,
                                       const simd::float4x4 &view)
{
    rebuild();
    if (!renderable) return;
    renderable->draw(encoder, projection, view);
}

void RoundedRectanglePrimitive::onColorChanged()
{
    if (renderable)
    {
        if (auto material = renderable->getMaterial())
        {
            material->setColor(getColor());
        }
    }
}

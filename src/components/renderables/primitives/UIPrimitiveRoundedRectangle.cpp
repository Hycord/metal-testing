#include "components/renderables/primitives/UIPrimitiveRoundedRectangle.h"
#include "utils/Math.h"
#include "systems/input/InputState.h"
#include "factories/RoundedRectangleFactory.h"

RoundedRectangleUIPrimitive::RoundedRectangleUIPrimitive(MTL::Device* device,
                                                         float left, float top,
                                                         float w, float h,
                                                         float radius,
                                                         const simd::float4 &col,
                                                         int qualityPerCorner)
    : device(device), l(left), t(top), width(w), height(h), rad(radius), perCorner(qualityPerCorner < 1 ? 1 : qualityPerCorner)
{
    color = col;
    // We'll emit explicit triangle indices; use triangle list primitive
    primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
}

void RoundedRectangleUIPrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    // Clamp radius to fit inside width/height
    float maxRad = 0.5f * std::min(width, height);
    float r = std::max(0.0f, std::min(rad, maxRad));

    // Build perimeter using factory in local space [0..width]x[0..height]
    RoundedRectangleFactory poly(simd::float2{width, height}, r, perCorner);
    int N = poly.pointCount(); // unique perimeter points

    // Vertex layout: [0] center, [1..N] perimeter points (with [N] == first perimeter point to close the fan)
    std::vector<Vertex> vertices(1 + N + 1);

    // Center of rectangle for triangle fan
    float cx = l + width * 0.5f;
    float cy = t + height * 0.5f;
    vertices[0] = {{cx, cy, 0.0f}, {color.x, color.y, color.z}};

    // Perimeter points in clockwise order; add first again at the end to close
    for (int i = 0; i <= N; ++i) {
        int idx = (i == N) ? 0 : i;
        simd::float2 p = poly.getPoint(idx);
        float x = l + p.x;
        float y = t + p.y;
        vertices[i + 1] = {{x, y, 0.0f}, {color.x, color.y, color.z}};
    }

    // Indices for triangle fan implemented as a triangle list
    std::vector<ushort> indices(N * 3);
    for (int i = 0; i < N; ++i) {
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

void RoundedRectangleUIPrimitive::draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho)
{
    rebuild();
    if (!renderable) return;
    renderable->draw(encoder, ortho, MetalMath::identity());
}

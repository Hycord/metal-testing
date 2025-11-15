#include "UIPrimitive.h"
#include "../Utils/Math.h"
#include "../Input/InputState.h"
#include <cmath>

// RectangleUIPrimitive implementation
RectangleUIPrimitive::RectangleUIPrimitive(MTL::Device* device, float left, float top, float w, float h, const simd::float4 &col)
    : device(device), l(left), t(top), width(w), height(h)
{
    color = col;
}

void RectangleUIPrimitive::ensureMesh()
{
    if (!dirty && mesh.vertexBuffer && renderable && renderable->getPrimitiveType() == primitiveType)
        return;

    // Build rectangle geometry according to primitive type
    // Vertex order: 0=lb,1=rb,2=rt,3=lt in pixel space
    float left = l;
    float top = t;
    float right = l + width;
    float bottom = t + height;

    Vertex vertices[4] = {
        {{left,  bottom, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{right, bottom, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{right, top,    0.0f}, {1.0f, 1.0f, 1.0f}},
        {{left,  top,    0.0f}, {1.0f, 1.0f, 1.0f}},
    };

    std::vector<ushort> indices;
    if (primitiveType == MTL::PrimitiveType::PrimitiveTypeTriangleStrip) {
        // 0-1-3-2 renders two triangles in strip: (0,1,3) and (3,1,2)
        ushort idx[] = {0, 1, 3, 2};
        indices.assign(idx, idx + 4);
    } else if (primitiveType == MTL::PrimitiveType::PrimitiveTypeLineStrip) {
        // Outline rectangle as a line strip: 0-1-2-3-0
        ushort idx[] = {0, 1, 2, 3, 0};
        indices.assign(idx, idx + 5);
    } else {
        // Default to triangle list
        ushort idx[] = {0, 1, 2, 2, 3, 0};
        indices.assign(idx, idx + 6);
    }

    // Allocate buffers
    if (!mesh.vertexBuffer) {
        mesh.vertexBuffer = device->newBuffer(4 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    }
    memcpy(mesh.vertexBuffer->contents(), vertices, 4 * sizeof(Vertex));
    mesh.vertexCount = 4;

    if (!indices.empty()) {
        size_t ibSize = indices.size() * sizeof(ushort);
        if (!mesh.indexBuffer || mesh.indexBuffer->length() < ibSize) {
            if (mesh.indexBuffer) mesh.indexBuffer->release();
            mesh.indexBuffer = device->newBuffer(ibSize, MTL::ResourceStorageModeShared);
        }
        memcpy(mesh.indexBuffer->contents(), indices.data(), ibSize);
        mesh.indexCount = indices.size();
    }

    // Vertex descriptor
    if (!mesh.vertexDescriptor) {
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
    }

    // (Re)create renderable with the chosen primitive type
    renderable = makeRenderable(device, mesh, color, primitiveType);
    dirty = false;
}

void RectangleUIPrimitive::draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho)
{
    ensureMesh();
    if (!renderable) return;
    renderable->draw(encoder, ortho, MetalMath::identity());
}

// CircleUIPrimitive implementation
CircleUIPrimitive::CircleUIPrimitive(MTL::Device* device, float x, float y, float r, const simd::float4 &col, int segs)
    : device(device), cx(x), cy(y), radius(r), segments(segs)
{
    color = col;
    primitiveType = MTL::PrimitiveType::PrimitiveTypeTriangle;
}

void CircleUIPrimitive::rebuild()
{
    if (!dirty && mesh.vertexBuffer)
        return;

    // Triangle fan: center + segments around
    int vertCount = segments + 2; // center + one per segment + repeat first
    std::vector<Vertex> vertices(vertCount);
    std::vector<ushort> indices((segments + 1) * 3);

    vertices[0] = {{cx, cy, 0.0f}, {color.x, color.y, color.z}};
    for (int i = 0; i <= segments; ++i) {
        float theta = (float(i) / float(segments)) * 2.0f * float(M_PI);
        float x = cx + radius * std::cos(theta);
        float y = cy + radius * std::sin(theta);
        vertices[i + 1] = {{x, y, 0.0f}, {color.x, color.y, color.z}};
    }

    // indices for triangle fan
    for (int i = 0; i < segments; ++i) {
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

void CircleUIPrimitive::draw(MTL::RenderCommandEncoder* encoder, const simd::float4x4 &ortho)
{
    rebuild();
    if (!renderable) return;
    renderable->draw(encoder, ortho, MetalMath::identity());
}

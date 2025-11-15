#include "components/renderables/primitives/2d/RectanglePrimitive.h"
#include "utils/Math.h"
#include "systems/input/InputState.h"
#include <cmath>


RectanglePrimitive::RectanglePrimitive(MTL::Device* device, float left, float top, float w, float h, const simd::float4 &col)
    : device(device), l(left), t(top), width(w), height(h)
{
    setColor(col);
}

void RectanglePrimitive::ensureMesh()
{
    if (!dirty && mesh.vertexBuffer && renderable && renderable->getPrimitiveType() == getPrimitiveType())
        return;

    
    
    float left = l;
    float top = t;
    float right = l + width;
    float bottom = t + height;

    Vertex vertices[4] = {
        {{left,  bottom, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{right, bottom, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{right, top,    0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{left,  top,    0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    };

    std::vector<ushort> indices;
    if (getPrimitiveType() == MTL::PrimitiveType::PrimitiveTypeTriangleStrip) {
        
        ushort idx[] = {0, 1, 3, 2};
        indices.assign(idx, idx + 4);
    } else if (getPrimitiveType() == MTL::PrimitiveType::PrimitiveTypeLineStrip) {
        
        ushort idx[] = {0, 1, 2, 3, 0};
        indices.assign(idx, idx + 5);
    } else {
        
        ushort idx[] = {0, 1, 2, 2, 3, 0};
        indices.assign(idx, idx + 6);
    }

    
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
        auto uvDescriptor = attributes->object(2);
        uvDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
        uvDescriptor->setBufferIndex(0);
        uvDescriptor->setOffset(offsetof(Vertex, uv));
        auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
        layoutDescriptor->setStride(sizeof(Vertex));
        mesh.vertexDescriptor = vertexDescriptor;
    }

    
    renderable = makeRenderable(device, mesh, color, getPrimitiveType());
    dirty = false;
}

void RectanglePrimitive::draw(MTL::RenderCommandEncoder* encoder,
                                const simd::float4x4 &projection,
                                const simd::float4x4 &view)
{
    ensureMesh();
    if (!renderable) return;
    renderable->draw(encoder, projection, view);
}

void RectanglePrimitive::onColorChanged()
{
    if (renderable)
    {
        if (auto material = renderable->getMaterial())
        {
            material->setColor(getColor());
        }
    }
}

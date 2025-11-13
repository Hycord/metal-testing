#include "factories/MeshFactory.h"
#include "core/LogManager.h"

MTL::Buffer *MeshFactory::buildTriangle(MTL::Device *device)
{
    LOG_START("MeshFactory: buildTriangle");

    Vertex verticies[3] = {
        {{-0.5, -0.5, 0.0}, {1.0, 1.0, 0.0}},
        {{0.5, -0.5, 0.0}, {1.0, 1.0, 0.0}},
        {{0.0, 0.5, 0.0}, {1.0, 1.0, 0.0}}};

    MTL::Buffer *buffer = device->newBuffer(3 * sizeof(Vertex), MTL::ResourceStorageModeShared);

    memcpy(buffer->contents(), verticies, 3 * sizeof(Vertex));

    LOG_FINISH("MeshFactory: buildTriangle");
    return buffer;
}

Mesh MeshFactory::buildQuad(MTL::Device *device)
{
    LOG_START("MeshFactory: buildQuad");
    Mesh mesh;
    Vertex verticies[4] = {
        {{-0.75, -0.75, 0.0}, {1.0, 0.0, 0.0}},
        {{0.75, -0.75, 0.0}, {0.0, 1.0, 0.0}},
        {{0.75, 0.75, 0.0}, {0.0, 0.0, 1.0}},
        {{-0.75, 0.75, 0.0}, {0.0, 1.0, 0.0}}};

    ushort indices[6] = {
        0, 1, 2,
        2, 3, 0};

    mesh.vertexBuffer = device->newBuffer(4 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), verticies, 4 * sizeof(Vertex));

    mesh.indexBuffer = device->newBuffer(6 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 6 * sizeof(ushort));

    mesh.vertexCount = 4;
    mesh.indexCount = 6;

    // Vertex descriptor
    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();

    // attribute 0: Position (vec2)
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    positionDescriptor->setOffset(offsetof(Vertex, position));
    positionDescriptor->setBufferIndex(0);
    // attribute 1: Color (vec3)
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setBufferIndex(0);
    colorDescriptor->setOffset(offsetof(Vertex, color));

    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(sizeof(Vertex));

    mesh.vertexDescriptor = vertexDescriptor;

    mesh.vertexCount = 4;
    mesh.indexCount = 6;

    LOG_FINISH("MeshFactory: buildQuad");
    return mesh;
}

Mesh MeshFactory::buildCube(MTL::Device *device)
{
    LOG_START("MeshFactory: buildCube");
    Mesh mesh;

    Vertex vertices[8] = {
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.5f, 0.2f}},
        {{-0.5f, 0.5f, 0.5f}, {0.2f, 0.7f, 0.3f}}
    };

    // 12 triangles, 36 indices
    ushort indices[36] = {
        // back
        0,1,2, 2,3,0,
        // front
        4,6,5, 6,4,7,
        // left
        4,0,3, 3,7,4,
        // right
        1,5,6, 6,2,1,
        // bottom
        4,5,1, 1,0,4,
        // top
        3,2,6, 6,7,3
    };

    mesh.vertexBuffer = device->newBuffer(8 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices, 8 * sizeof(Vertex));

    mesh.indexBuffer = device->newBuffer(36 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 36 * sizeof(ushort));

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

    mesh.vertexCount = 8;
    mesh.indexCount = 36;

    LOG_FINISH("MeshFactory: buildCube");
    return mesh;
}

Mesh MeshFactory::buildScreenQuad(MTL::Device *device, float left, float top, float width, float height)
{
    LOG_START("MeshFactory: buildScreenQuad");
    Mesh mesh;

    // Coordinates in pixel space. We'll expect an orthographic projection with same pixel extents.
    float l = left;
    float t = top;
    float r = left + width;
    float b = top + height;

    // Use neutral white vertex colors for screen-space quads so the material
    // (uniform color) controls the visible color instead of per-vertex tinting.
    Vertex verticies[4] = {
        {{l, b, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{r, b, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{r, t, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{l, t, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    ushort indices[6] = {0,1,2, 2,3,0};

    mesh.vertexBuffer = device->newBuffer(4 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), verticies, 4 * sizeof(Vertex));

    mesh.indexBuffer = device->newBuffer(6 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 6 * sizeof(ushort));

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

    LOG_FINISH("MeshFactory: buildScreenQuad");
    return mesh;
}

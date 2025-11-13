#include "MeshFactory.h"
#include "../LogManager/LogManager.h"

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

    LOG_FINISH("MeshFactory: buildQuad");
    return mesh;
}
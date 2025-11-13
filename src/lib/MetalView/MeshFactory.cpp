#include "MeshFactory.h"

MTL::Buffer *MeshFactory::buildTriangle(MTL::Device *device)
{

    Vertex verticies[3] = {
        {{-0.5, -0.5}, {1.0, 1.0, 0.0}},
        {{0.5, -0.5}, {1.0, 1.0, 0.0}},
        {{0.0, 0.5}, {1.0, 1.0, 0.0}}};

    MTL::Buffer *buffer = device->newBuffer(3 * sizeof(Vertex), MTL::ResourceStorageModeShared);

    memcpy(buffer->contents(), verticies, 3 * sizeof(Vertex));

    return buffer;
}

Mesh MeshFactory::buildQuad(MTL::Device *device)
{
    Mesh mesh;
    Vertex verticies[4] = {
        {{-0.75, -0.75}, {1.0, 0.0, 0.0}},
        {{0.75, -0.75}, {0.0, 1.0, 0.0}},
        {{0.75, 0.75}, {0.0, 0.0, 1.0}},
        {{-0.75, 0.75}, {0.0, 1.0, 0.0}}};

    ushort indices[6] = {
        0, 1, 2,
        2, 3, 0};

    mesh.vertexBuffer = device->newBuffer(4 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), verticies, 4 * sizeof(Vertex));

    mesh.indexBuffer = device->newBuffer(6 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 6 * sizeof(ushort));

    return mesh;
}
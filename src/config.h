
#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

#include <iostream>
#include <sstream>
#include <fstream>


#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO
// #define ENABLE_LOG_DEBUG

struct Vertex
{
    simd::float3 position;
    simd::float3 color;
};


struct Mesh
{
    MTL::Buffer *vertexBuffer, *indexBuffer;
    MTL::VertexDescriptor* vertexDescriptor;
    // how many vertices/indices to draw
    size_t vertexCount;
    size_t indexCount;
};

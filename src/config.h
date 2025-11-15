
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


struct Vertex
{
    simd::float3 position;
    simd::float3 color;
    simd::float2 uv;
};


struct Mesh
{
    MTL::Buffer *vertexBuffer = nullptr;
    MTL::Buffer *indexBuffer = nullptr;
    MTL::VertexDescriptor* vertexDescriptor = nullptr;
    
    size_t vertexCount = 0;
    size_t indexCount = 0;
};

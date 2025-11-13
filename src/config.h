
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

struct Vertex
{
    simd::float3 position;
    simd::float3 color;
};
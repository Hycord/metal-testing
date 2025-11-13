
#pragma once
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

#include <iostream>
#include <sstream>
#include <fstream>

struct Vertex
{
    simd::float2 position;
    simd::float3 color;
};
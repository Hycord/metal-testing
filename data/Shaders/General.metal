#include <metal_stdlib>
using namespace metal;

struct VertexInput
{
    float3 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct VertexOutput
{
    float4 position [[position]];
    half3 color;
};

VertexOutput vertex vertexGeneral(
    VertexInput input [[stage_in]],
    constant float4x4 &transform [[buffer(1)]],
    constant float4x4 &projection [[buffer(2)]],
    constant float4x4 &view [[buffer(3)]])
{

    VertexOutput payload;

    half3 pos = half3(input.position);

    payload.position = float4(half4x4(projection) * half4x4(view) * half4x4(transform) * half4(pos, 1.0));
    payload.color = half3(input.color);
    return payload;
}

// fragment buffer(0) is reserved for a material color (float4)
half4 fragment fragmentGeneral(VertexOutput frag [[stage_in]], constant float4 &materialColor [[buffer(0)]])
{
    // Multiply vertex color by the material color so materials can tint meshes and
    // screen-space quads. The material's alpha controls the output alpha.
    half3 col = frag.color * half3(materialColor.xyz);
    half a = half(materialColor.w);
    return half4(col, a);
}
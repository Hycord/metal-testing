#include <metal_stdlib>
using namespace metal;

struct VertexInput
{
    float3 position [[attribute(0)]];
    float3 color [[attribute(1)]];
    float2 uv [[attribute(2)]];
};

struct VertexOutput
{
    float4 position [[position]];
    half3 color;
    float2 uv;
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
    payload.uv = input.uv;
    return payload;
}


half4 fragment fragmentGeneral(
    VertexOutput frag [[stage_in]], 
    constant float4 &materialColor [[buffer(0)]],
    texture2d<float> tex [[texture(0)]],
    sampler samp [[sampler(0)]])
{
    
    float4 texColor = tex.sample(samp, frag.uv);
    
    
    
    if (texColor.r == 0.0 && texColor.g == 0.0 && texColor.b == 0.0 && texColor.a == 0.0) {
        texColor = float4(1.0, 1.0, 1.0, 1.0);
    }
    
    
    half3 col = frag.color * half3(texColor.rgb) * half3(materialColor.xyz);
    half a = half(texColor.a * materialColor.w);
    
    return half4(col, a);
}
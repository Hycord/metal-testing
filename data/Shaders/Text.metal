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

VertexOutput vertex vertexText(
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



half4 fragment fragmentText(
    VertexOutput frag [[stage_in]], 
    constant float4 &materialColor [[buffer(0)]],
    texture2d<float> fontAtlas [[texture(0)]],
    sampler samp [[sampler(0)]])
{
    
    float alpha = fontAtlas.sample(samp, frag.uv).r;
    
    
    if (alpha < 0.01) {
        discard_fragment();
    }
    
    
    half3 textColor = frag.color * half3(materialColor.xyz);
    
    
    return half4(textColor, half(alpha * materialColor.w));
}

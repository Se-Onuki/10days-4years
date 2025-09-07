#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    result.color_ = gTexture.Sample(gSampler, input.texCoord_);
    result.color_.a = 1.f;
    
    return result;
}
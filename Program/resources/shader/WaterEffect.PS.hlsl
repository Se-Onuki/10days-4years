#include "FullScreen.hlsli"

// 元の水パーティクルのテクスチャ
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 出力
struct PixelShaderOutput
{
    float4 color_ : SV_Target0;
};

// 固定値のパラメータ（cbufferを使わない）
static const float Thickness = 0.1f; // しきい値
static const float4 MaskColor = float4(0.0f, 0.5f, 1.0f, 1.0f); // 水色

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;

    // 元テクスチャの色を取得
    float4 texColor = gTexture.Sample(gSampler, input.texCoord_);
    
    // Stepでマスクを作成（色がしきい値以上なら1、未満なら0）
    const float value = step(Thickness, texColor.b);
    if (value == 0)
    {
        discard;
    }
    
    
    result.color_ = MaskColor;
 
    return result;
}
#include "FullScreen.hlsli"

// 元の水パーティクルのテクスチャ
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 出力
struct PixelShaderOutput
{
    float32_t4 color_ : SV_Target0;
};

// 固定値のパラメータ（cbufferを使わない）
static const float Thickness = 0.1f; // しきい値
static const float4 MaskColor = float4(0.0f, 0.5f, 1.0f, 1.0f); // 水色

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;

    // 元テクスチャの色を取得
    float4 texColor = gTexture.Sample(gSampler, input.texCoord_);

    // Stepでマスクを作成（アルファがしきい値以上なら1、未満なら0）
    result.color_ = step(Thickness, texColor) * MaskColor;
    
    result.color_.a = 1.f; // アルファをマスクに置き換え
    return result;
}
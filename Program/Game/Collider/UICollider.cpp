#include "UICollider.h"

bool UICollider::IsMouseOverRotatedRect(const Vector2& rectCenter, float rotationRadians, const Vector2& size){
    Vector2 mousePos = ImVec2toSolVec(ImGui::GetMousePos());

    // 四角形の中心を基準に相対座標
    Vector2 boxCenter = Vector2{ rectCenter.x,rectCenter.y };
    Vector2 relative = mousePos - boxCenter;

    // 逆回転でローカル空間に変換
    float cosR = std::cos(-rotationRadians);
    float sinR = std::sin(-rotationRadians);

    Vector2 localPos = {
        relative.x * cosR - relative.y * sinR,
        relative.x * sinR + relative.y * cosR
    };

    // 半サイズ
    float halfWidth = size.x * 0.5f;
    float halfHeight = size.y * 0.5f;

    // AABB 判定（ローカル空間内での矩形チェック）
    return (
        localPos.x >= -halfWidth && localPos.x <= halfWidth &&
        localPos.y >= -halfHeight && localPos.y <= halfHeight
        );
}

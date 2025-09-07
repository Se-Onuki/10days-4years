#pragma once
#include<../User/AoMidori.h>
//UIの当たりをまとめる(予定)
//マウスだったり、矩形の当たり同士もここに記述したい

namespace UICollider {
	bool IsMouseOverRotatedRect(const Vector2& rectCenter, float rotationRadians, const Vector2& size);
};


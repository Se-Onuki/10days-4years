#include "SplashParticle.h"

void TD_10days::SplashParticle::Init(const Vector2& position, const Vector2& velocity)
{
	sprite_ = Sprite::Generate(TextureManager::Load("splash.png"));
	sprite_->SetPivot({ 0.5f, 0.5f });
	position_ = position;
	sprite_->SetPosition(position_);

	// velocity の方向ベクトル
	Vector2 dir = velocity;
	dir.Normalize();

	// 方向ベクトルを角度に変換
	float baseAngle = atan2(dir.y, dir.x);

	// ±30度ランダムにずらす
	float angleOffset = SoLib::Random::GetRandom(-30.0f, 30.0f) * (SoLib::Angle::PI / 180.0f);
	float angle = baseAngle + angleOffset;

	// ランダムな速さ（粒子感を出すため）
	float speed = SoLib::Random::GetRandom(0.1f, 0.15f);

	velocity_ = {
		cos(angle) * speed,
		sin(angle) * speed
	};

	// サイズと色
	float size = SoLib::Random::GetRandom(size_.min, size_.max);
	sprite_->SetScale({ size, size });
	sprite_->SetColor({ 0.0f, 0.0f, 1.0f, 1.0f }); // 水色

	isActive_ = true;
}

void TD_10days::SplashParticle::Update(float deltaTime, float gravity)
{
	if (!isActive_) {
		return;
	}
	// 経過時間
	age_ += deltaTime;
	if (age_ >= lifeTime_) {
		isActive_ = false;
		return;
	}

	// 物理挙動
	velocity_.y += gravity;
	position_ += velocity_;
	sprite_->SetPosition(position_);

	// 進行度 (0.0 → 1.0)
	float t = age_ / lifeTime_;

	// スケール縮小
	float scale = startScale_ * (1.0f - t);
	sprite_->SetScale({ scale, scale });

	// 色補間 (水色 → 白 → 消える)
	Vector4 color;
	color.x = startColor_.x + (endColor_.x - startColor_.x) * t;
	color.y = startColor_.y + (endColor_.y - startColor_.y) * t;
	color.z = startColor_.z + (endColor_.z - startColor_.z) * t;
	color.w = startColor_.w + (endColor_.w - startColor_.w) * t;
	sprite_->SetColor(color);
}

void TD_10days::SplashParticle::Draw()
{
	if (isActive_) {
		sprite_->Draw();
	}
}

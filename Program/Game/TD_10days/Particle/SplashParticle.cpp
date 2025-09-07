#include "SplashParticle.h"

void TD_10days::SplashParticle::Init(const Vector2& position, const Vector2& velocity)
{
	sprite_ = Sprite::Generate(TextureManager::Load("output1.png"));
	sprite_->SetPivot(Vector2{ 0.5f, 0.5f });
	position_ = position;
	startPosition_ = position;
	sprite_->SetPosition(position_);
	velocity_ = Vector2{ velocity.x * 0.05f + SoLib::Random::GetRandom(-0.1f, 0.1f),  -fabs(velocity.y) * 0.05f - SoLib::Random::GetRandom(0.05f, 0.15f) };
	float size = SoLib::Random::GetRandom(size_.min, size_.max);
	sprite_->SetScale(Vector2{ size, size });
	sprite_->SetColor(/*Vector4{ 0.0f, 0.5f, 1.0f, 1.0f }*/Vector4{ 0.0f, 0.0f, 1.0f, 1.0f }); // 薄い水色
}

void TD_10days::SplashParticle::Update(float deltaTime, float gravity)
{
	if (isActive_) {
		velocity_.y += gravity;
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

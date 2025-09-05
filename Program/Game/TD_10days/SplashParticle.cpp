#include "SplashParticle.h"

void TD_10days::SplashParticle::Init(const Vector2& position, const Vector2& velocity)
{
	sprite_ = Sprite::Generate(TextureManager::Load("output1.png"));
	sprite_->SetPivot(Vector2{ 0.5f, 0.5f });
	position_ = position;
	startPosition_ = position;
	sprite_->SetPosition(position_);
	velocity_ = Vector2{ velocity.x * 0.3f + SoLib::Random::GetRandom(-1.0f, 1.0f),  -fabs(velocity.y) * 0.5f - SoLib::Random::GetRandom(1.0f, 3.0f) };
	float size = SoLib::Random::GetRandom(size_.min, size_.max);
	sprite_->SetScale(Vector2{ size, size });
	sprite_->SetColor(Vector4{ 0.0f, 0.5f, 1.0f, 1.0f }); // 薄い水色
}

void TD_10days::SplashParticle::Update(float gravity)
{
	if (isActive_) {
		velocity_.y += gravity;
	}
	position_ += velocity_;
	sprite_->SetPosition(position_);
}

void TD_10days::SplashParticle::Draw()
{
	if (isActive_) {
		sprite_->Draw();
	}
}

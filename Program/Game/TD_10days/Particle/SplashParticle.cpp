#include "SplashParticle.h"

void TD_10days::SplashParticle::Init(const Vector2& position, const Vector2& velocity)
{
	sprite_ = Sprite::Generate(TextureManager::Load(/*"output1.png"*/"splash.png"));
	sprite_->SetPivot({ 0.5f, 0.5f });
	position_ = position;
	sprite_->SetPosition(position_);

	// プレイヤー速度から方向ベクトルを作成（逆方向）
	Vector2 dir = velocity;
	dir.Normalize();

	// ランダムで±30度広げる
	float angle = atan2(dir.y, dir.x);

	// 横方向の動きが強いなら → Y方向に広がりを持たせる
	if (fabs(dir.x) > fabs(dir.y)) {
		// 横ベース
		angle += SoLib::Random::GetRandom(-30.0f, 30.0f) * (SoLib::Angle::PI / 180.0f);

		velocity_ = {
			cos(angle) * SoLib::Random::GetRandom(0.1f, 0.3f),
			sin(angle) *  SoLib::Random::GetRandom(0.2f, 0.5f)
		};

	}
	else {
		// 縦ベース（今まで通り）
		angle += SoLib::Random::GetRandom(-30.0f, 30.0f) * (SoLib::Angle::PI / 180.0f);

		velocity_ = {
		        cos(angle) * SoLib::Random::GetRandom(0.1f, 0.25f),
		        sin(angle) * SoLib::Random::GetRandom(0.08f, 0.1f)
		};

		if (dir.y < 0.0f) {
			velocity_.y = -sin(angle) * SoLib::Random::GetRandom(0.01f, 0.05f);
		}
	}

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

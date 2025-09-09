#include "BackgroundParticle.h"

namespace TD_10days {
	void BackgroundParticle::Init(const SolEngine::Camera2D& camera)
	{
		sprite_ = Sprite::Generate(TextureManager::Load("splash.png"));
		sprite_->SetPivot({ 0.5f, 0.5f });

		// カメラの範囲を計算
		Vector3 camPos = camera.translation_;
		float halfW = (camera.windowSize_.x * 0.5f) * camera.scale_;
		float halfH = (camera.windowSize_.y * 0.5f) * camera.scale_;

		float left = camPos.x - halfW;
		float right = camPos.x + halfW;
		float top = camPos.y + halfH;
		//float bottom = camPos.y - halfH;

		// 画面上端〜少し上からランダムで出現
		position_ = {
			SoLib::Random::GetRandom(left, right),
			SoLib::Random::GetRandom(0.0f, top) // 少し上から落ちる
		};

		velocity_ = {
			SoLib::Random::GetRandom(-0.05f, 0.05f),
			SoLib::Random::GetRandom(-0.01f, -0.03f)
		};

		float size = SoLib::Random::GetRandom(0.05f, 0.1f);
		sprite_->SetScale({ size, size });

		startColor_ = { 0.2f, 0.7f, 0.5f, 0.0f };
		midColor_ =   { 0.2f, 0.7f, 0.5f, 0.8f };
		endColor_ =   { 0.2f, 0.7f, 0.5f, 0.0f };

		/*startColor_ = { 0.353f, 0.859f, 0.745f, 0.0f };
		midColor_ = { 0.353f, 0.859f, 0.745f, 0.8f };
		endColor_ = { 0.353f, 0.859f, 0.745f, 0.0f };*/

		sprite_->SetColor(startColor_);

		age_ = 0.0f;
		lifeTime_ = SoLib::Random::GetRandom(2.0f, 4.0f);

		sprite_->SetPosition(position_);
		isActive_ = true;
	}

	void BackgroundParticle::Update(float deltaTime) {
		if (!isActive_) return;

		age_ += deltaTime;
		if (age_ >= lifeTime_) {
			Init(*pCamera_); // 再利用
			return;
		}

		// 移動
		position_ += velocity_;
		sprite_->SetPosition(position_);

		// 進行度 (0.0 → 1.0)
		float t = age_ / lifeTime_;

		// アルファ補間（前半フェードイン → 後半フェードアウト）
		Vector4 color;
		if (t < 0.5f) {
			float f = t / 0.5f; // 0.0 → 1.0
			color.x = startColor_.x + (midColor_.x - startColor_.x) * f;
			color.y = startColor_.y + (midColor_.y - startColor_.y) * f;
			color.z = startColor_.z + (midColor_.z - startColor_.z) * f;
			color.w = startColor_.w + (midColor_.w - startColor_.w) * f;
		}
		else {
			float f = (t - 0.5f) / 0.5f; // 0.0 → 1.0
			color.x = midColor_.x + (endColor_.x - midColor_.x) * f;
			color.y = midColor_.y + (endColor_.y - midColor_.y) * f;
			color.z = midColor_.z + (endColor_.z - midColor_.z) * f;
			color.w = midColor_.w + (endColor_.w - midColor_.w) * f;
		}

		sprite_->SetColor(color);
	}

	void BackgroundParticle::Draw() {
		if (isActive_) { sprite_->Draw(); }
	}
}


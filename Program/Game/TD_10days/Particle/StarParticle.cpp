#include "StarParticle.h"

namespace TD_10days {
	
	void StarParticle::Init()
	{
		sprite_ = Sprite::Generate(TextureManager::Load("Star.png"));
		sprite_->SetPivot({ 0.5f, 0.5f });

		
		//float bottom = camPos.y - halfH;

		// 画面上端〜少し上からランダムで出現
		position_ = {
			SoLib::Random::GetRandom(minPosition_.x, maxPosition_.x),
			SoLib::Random::GetRandom(minPosition_.y, maxPosition_.y) // 少し上から落ちる
		};

		size_ = SoLib::Random::GetRandom(5.0f, 30.0f);
		sprite_->SetScale({ 0.0f, 0.0f });


		age_ = 0.0f;
		lifeTime_ = SoLib::Random::GetRandom(2.0f, 4.0f);

		sprite_->SetPosition(position_);
		isActive_ = true;
	}

	void StarParticle::Update(float deltaTime) {
		if (!isActive_) return;

		age_ += deltaTime;
		if (age_ >= lifeTime_) {
			Init(); // 再利用
			return;
		}


		// 進行度 (0.0 → 1.0)
		float t = age_ / lifeTime_;

		float scale = 0.0f;

		if (t < 0.5f) {
			// 前半：0 → size_
			float f = t / 0.5f; // 0.0 → 1.0
			// イージング (sin波で自然に)
			f = sinf(f * SoLib::Angle::PI * 0.5f);
			scale = size_ * f;
		}
		else {
			// 後半：size_ → 0
			float f = (t - 0.5f) / 0.5f; // 0.0 → 1.0
			// イージング (逆向き sin 波)
			f = 1.0f - sinf(f * SoLib::Angle::PI * 0.5f);
			scale = size_ * f;
		}

		sprite_->SetScale({ scale, scale });
	}

	void StarParticle::Draw() {
		if (isActive_) { sprite_->Draw(); }
	}
}


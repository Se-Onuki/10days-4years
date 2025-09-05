#pragma once


#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>
#include <algorithm>

#include "../Engine/DirectBase/Input/Input.h"
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャの情報
#include "../../Engine/Utils/Math/Random.h"
#include "../../Engine/DirectBase/File/VariantItem.h"		// 外部編集可能な変数の定義
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace TD_10days {
	class SplashParticle {
		struct Size {
			float min;
			float max;
		};
	public:
		SplashParticle() = default;
		~SplashParticle() = default;
		void Init(const Vector2& position, const Vector2& velocity);
		void Update(float gravity);
		void Draw();

		bool GetActive() const { return isActive_; }
		Vector2 GetPosition() const { return position_; }
		Vector2 GetVelocity() const { return velocity_; }

		void SetActive(bool isActive) { isActive_ = isActive; }
		void SetPosition(const Vector2& position) { position_ = position; }
		void SetVelocity(const Vector2& velocity) { velocity_ = velocity; }
		void SetColor(const Vector4& color) { sprite_->SetColor(color); }


	private:
		std::unique_ptr<Sprite> sprite_;
		Vector2 velocity_{};
		Vector2 position_{};
		Vector2 startPosition_{};
		const Size size_{ 10.0f, 20.0f };
		bool isActive_ = true;
	};
}

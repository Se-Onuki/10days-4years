#pragma once

#include "../../Engine/DirectBase/Base/TextureManager.h"
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace TD_10days {
	class CountUI {
	public:
		CountUI() = default;
		~CountUI() = default;
		void Init();
		void Update(float deltaTime, const Vector2& position);
		void Draw();

		void SetIsActive(bool isActive) { isActive_ = isActive; }
		void SetPostion(const Vector2& position) { position_ = position; }
		void SetTime(float time) { time_ = time; }

	private:
		bool isActive_ = false;
		std::vector<std::unique_ptr<Sprite>> number_;
		const float count_ = 9.0f;
		float time_;
		const float size_ = 0.4f;
		const Vector2 offset{ 0.0f, 1.0f };
		Vector2 position_;
	};
}
#pragma once

#include "../../Engine/DirectBase/Base/TextureManager.h"
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace TD_10days {
	class CountUI {
	public:
		CountUI() = default;
		~CountUI() = default;
		void Init();
		void Update(const Vector2& position);
		void Draw();

		void SetPostion(const Vector2& position) { position_ = position; }
		void SetTime(float time) { time_ = time; }

	private:
		std::vector<std::unique_ptr<Sprite>> onesNumber_;
		std::vector<std::unique_ptr<Sprite>> tensNumber_;
		float time_;
		const float size_ = 0.4f;
		const Vector2 offset{ 0.0f, 1.0f };
		Vector2 position_;
	};
}
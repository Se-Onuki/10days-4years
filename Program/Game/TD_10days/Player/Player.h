#pragma once
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../../../Engine/DirectBase/Render/Camera.h"

namespace TD_10days {

	class Player {
	public:

		Player() = default;

		void Init();
		void Update(float deltaTime);

		void Draw() const;


	private:

		void CalcSprite();

		void MoveUpdate(float deltaTime);



	private:
		// 座標
		Vector2 position_{};
		// 移動量
		Vector2 velocity_{};
		// 加速度
		Vector2 acceleration_{};

		Vector2 size_ = Vector2::one;

		// 描画スプライト
		std::unique_ptr<Sprite> sprite_;

	};
}
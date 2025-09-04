#pragma once
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "../LevelMapChip.h"

namespace TD_10days {

	class Player {
	public:

		Player() = default;

		void Init();
		void Update(float deltaTime);

		void Draw() const;

		void SetPosition(const Vector2 &position) { position_ = position; }

		void InputFunc();

		void SetHitBox(LevelMapChip::LevelMapChipHitBox *const pHitBox) { pHitBox_ = pHitBox; }

		Vector2 &GetPosition() { return position_; }

	private:

		void CalcSprite();

		std::tuple<float, Vector3> CalcMoveProgress(const Vector2 &velocity);

		void MoveUpdate(float deltaTime);

	private:
		// 座標
		Vector2 position_{};
		// 移動量
		Vector2 velocity_{};
		// 加速度
		Vector2 acceleration_{};

		Vector2 size_ = Vector2::one * 0.8f;

		// 描画スプライト
		std::unique_ptr<Sprite> sprite_;

		LevelMapChip::LevelMapChipHitBox *pHitBox_ = nullptr;

	};
}
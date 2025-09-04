#pragma once
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include "../LevelMapChip.h"
#include "Water.h"

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

		void SetWater(Water *water) { pWater_ = water; }

	private:

		/// @brief 画像の更新処理
		void CalcSprite();

		/// @brief 移動ベクトルに対する移動可能な進行度を計算する
		/// @param[in] velocity 移動ベクトル
		/// @return (進行度, 衝突した法線ベクトル)
		std::tuple<float, Vector3> CalcMoveProgress(const Vector2 &velocity);

		/// @brief 移動処理
		/// @param[in] deltaTime 前フレームからの経過時間
		void MoveUpdate(float deltaTime);

		bool IsInWater() const;

	private:

		Water *pWater_ = nullptr;

		std::string spriteName_ = "TD_10days/Player/Player.png";

		// 描画スプライト
		std::unique_ptr<Sprite> sprite_;
		// 当たり判定
		LevelMapChip::LevelMapChipHitBox *pHitBox_ = nullptr;

		// 座標
		Vector2 position_{};
		// 移動量
		Vector2 velocity_{};
		// 加速度
		Vector2 acceleration_{};

		Vector2 size_ = Vector2::one * 0.8f;

		Vector2 gravity_ = Vector2{ 0.f, -9.8f };

		bool isGround_ = false;
	};
}
#pragma once

#include <cstdint>
#include <memory>

#include "Utils/Math/Math.hpp"
#include "DirectBase/Render/Camera.h"

namespace TD_10days {

	class Player;
	class LevelMapChip;

	struct FocusParametor {

		// 目標となる座標
		Vector2 position_;

		// 範囲内に入っている時間
		float time_;

		// 設定された強度
		float focusPower_;
	};

	class FocusManager {
	public:
		FocusManager() = default;
		~FocusManager() = default;

		/// @brief マップチップの割当
		/// @param[in] levelMapChip 
		void SetLevelMapChip(LevelMapChip *levelMapChip) { pLevelMapChip_ = levelMapChip; }

		void Update(const float deltaTime);


	private:
		// マップチップの借用
		LevelMapChip *pLevelMapChip_;

		float timer_;
	};

	class FocusCamera
	{
	public:
		FocusCamera() = default;
		~FocusCamera() = default;


		/// @brief 初期化処理
		void Init();

		/// @brief 更新処理
		/// @param[in] deltaTime 差分
		void Update(const float deltaTime);

		/// @brief カメラの取得
		/// @return カメラのアドレス
		const auto *const GetCamera() const { return camera_.get(); }



	private:

		// カメラの実体
		std::unique_ptr<SolEngine::Camera2D> camera_;

		// 注視する実体
		Player *pFocusEntity_;

		// 可動範囲の下限
		Vector2 min_;
		// 可動範囲の上限
		Vector2 max_;

	};

}
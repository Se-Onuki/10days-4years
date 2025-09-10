#pragma once

#include <cstdint>
#include <memory>

#include "Utils/Math/Math.hpp"
#include "DirectBase/Render/Camera.h"
#include "Utils/SoLib/SoLib_Easing.h"

namespace TD_10days {

	class Player;
	class LevelMapChip;

	struct FocusParametor {

		// 目標となる座標
		Vector2 position_;

		// 範囲の広さ
		float focusRadius_;

		// 設定された強度
		float focusPower_;

		// イージングの関数
		SoLib::EaseFunc easing_;

		Vector2 CalcTargetPos(const float time, const Vector2 pos) const;
	};

	class FocusManager {
	public:
		FocusManager() = default;
		~FocusManager() = default;

		/// @brief マップチップの割当
		/// @param[in] levelMapChip マップチップのアドレス
		void SetLevelMapChip(LevelMapChip *levelMapChip) { pLevelMapChip_ = levelMapChip; }

		/// @brief プレイヤの割当
		/// @param[in] player プレイヤのアドレス
		void SetPlayer(Player *player) { pPlayer_ = player; }

		// 更新処理
		void Update(const float deltaTime);

		std::optional<Vector2> CalcTargetPos() const;

		std::pair<std::optional<FocusParametor>, float> GetParamAndTime() const { return { currentTarget_, timer_ }; }

	private:

		std::optional<Vector2> FindFocusTarget(const LevelMapChip &mapChip, const Vector2 pos) const;

	private:
		// マップチップの借用
		LevelMapChip *pLevelMapChip_;

		Player *pPlayer_ = nullptr;

		// 目標の注視点
		std::optional<FocusParametor> currentTarget_ = std::nullopt;

		float timer_;
	};

	class FocusCamera
	{
	public:
		FocusCamera() = default;
		~FocusCamera() = default;


		/// @brief 初期化処理
		void Init();

		void SetCameraPos(Vector2 pos);

		/// @brief 更新処理
		/// @param[in] deltaTime 差分
		void Update(const float deltaTime);

		/// @brief カメラの取得
		/// @return カメラのアドレス
		const auto *const GetCamera() const { return camera_.get(); }

		/// @brief プレイヤーのアドレスの設定
		/// @param[in] player プレイヤ
		void SetEntity(Player *player) { pFocusEntity_ = player; }

		void SetScale(size_t y, size_t x) { y_ = y; x_ = x; }

		Vector2 CalcTargetPoint();

		void CalcWindowSpan();

		void SetParamAndTime(const std::pair<std::optional<FocusParametor>, float> &params);

	private:
		// ステージのサイズ
		size_t y_, x_;

		// カメラの実体
		std::unique_ptr<SolEngine::Camera2D> camera_;

		// 注視する実体
		Player *pFocusEntity_;

		// プレイヤからの差分
		Vector2 playerDiff_;

		// 可動範囲の下限
		Vector2 min_;
		// 可動範囲の上限
		Vector2 max_;

		std::optional<Vector2> targetPos_;

		std::pair<std::optional<FocusParametor>, float> focusParams_;

	};

}
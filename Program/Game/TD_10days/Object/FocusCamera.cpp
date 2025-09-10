#include "FocusCamera.h"
#include "../LevelMapChip.h"
#include "Player.h"

namespace TD_10days {

	void FocusManager::Update(const float deltaTime) {

		// 時間の更新
		timer_ += deltaTime;

		const auto &lvMap = *pLevelMapChip_;
		const auto &player = *pPlayer_;

		// ターゲットの情報群
		const auto &focusTargets = lvMap.GetFocusPointData();
		if (focusTargets.empty()) { return; }

		// プレイヤの座標
		const Vector2 playerPos = player.GetPosition();

		// 算出した座標
		const auto &findPos = FindFocusTarget(lvMap, playerPos);

		// 算出した座標がないなら終わり
		if (not findPos) {
			// 全て空にする
			currentTarget_ = std::nullopt;
			timer_ = 0.f;
			return;
		}

		// もし既存の座標と一致したなら終わり
		if (currentTarget_ and currentTarget_->position_ == *findPos) {
			return;
		}

		// 目標地点のデータ
		const auto &focusData = focusTargets.at(*findPos);

		// 座標群からデータを作成する
		FocusParametor parametor = FocusParametor{
				.position_ = *findPos,
				.focusRadius_ = focusData.focusRadius_,
				.focusPower_ = focusData.focutPower_,
				.easing_ = focusData.easing_,
		};

		// データを書き込む
		currentTarget_ = parametor;

	}

	std::optional<Vector2> FocusManager::CalcTargetPos() const
	{
		//// もし目標地点がないならnullopt
		//if (not currentTarget_) {
		//	return std::nullopt;
		//}
		//// 算出した係数
		//const float t = std::clamp(timer_ / (currentTarget_->focusPower_), 0.f, 1.f);
		//// イージングを行った後の係数
		//const float lerpT = (currentTarget_->easing_)(t);



		return std::nullopt;
	}

	std::optional<Vector2> FocusManager::FindFocusTarget(const LevelMapChip &mapChip, const Vector2 pos) const
	{
		// フォーカスポイントの取得
		const auto &focusPoint = mapChip.GetFocusPointData();

		std::pair<std::optional<Vector2>, float> result{ std::nullopt, (std::numeric_limits<float>::max)() };

		for (const auto &
			[
				chipPos,	// 座標
				data	// 格納されたデータ
			] : focusPoint) {

			const float length = (chipPos - pos).LengthSQ();
			// プレイヤの位置から目標地点の位置が範囲内ならtrueとする
			const bool isInside = length < (data.focusRadius_ * data.focusRadius_);
			// 範囲外なら次へ
			if (not isInside) { continue; }

			// 既存のものより近かったらそれを返す
			if (result.second > length) {
				result.first = chipPos;
			}
		}

		return result.first;
	}

	void FocusCamera::Init() {
		camera_ = std::make_unique<SolEngine::Camera2D>();
		camera_->scale_ = 0.015f;
		camera_->Init();
	}

	void FocusCamera::SetCameraPos(Vector2 pos)
	{
		camera_->translation_.ToVec2() = pos;

		// カメラの更新処理
		camera_->UpdateMatrix();
	}

	void FocusCamera::Update(const float) {


		const auto &[target, time] = focusParams_;

		Vector2 targetPos = CalcTargetPoint();

		if (focusParams_.first) {
			targetPos = focusParams_.first->CalcTargetPos(focusParams_.second, targetPos);
		}
		targetPos = ClampCameraPos(targetPos);

		const Vector2 cameraPos = camera_->translation_.ToVec2();

		camera_->translation_.ToVec2() = SoLib::Lerp(targetPos, cameraPos, 0.9f);

		// カメラの更新処理
		camera_->UpdateMatrix();
	}

	Vector2 FocusCamera::CalcTargetPoint()
	{
		// プレイヤの位置からある程度位置を算出
		const Vector2 playerPos = pFocusEntity_->GetPosition();
		const Vector2 diff{ pFocusEntity_->GetVelocity().x * 0.25f,0.f };

		playerDiff_ = SoLib::Lerp(diff, playerDiff_, 0.5f);

		const Vector2 targetPos = ClampCameraPos(playerPos + playerDiff_);
		return targetPos;
	}

	Vector2 FocusCamera::ClampCameraPos(const Vector2 &pos) const
	{
		Vector2 result;
		result.x = std::clamp(pos.x, min_.x, max_.x);
		result.y = std::clamp(pos.y, min_.y, max_.y);
		return result;
	}

	void FocusCamera::CalcWindowSpan()
	{
		min_ = camera_->GetVisibleSize() / 2;
		max_ = Vector2{ static_cast<float>(x_), static_cast<float>(y_) } - (camera_->GetVisibleSize() / 2) - Vector2::one;
	}

	void FocusCamera::SetParamAndTime(const std::pair<std::optional<FocusParametor>, float> &params)
	{
		focusParams_ = params;
	}


	Vector2 FocusParametor::CalcTargetPos(const float time, const Vector2 pos) const
	{
		// 算出した係数
		const float t = std::clamp(time / (focusPower_), 0.f, 1.f);
		// イージングを行った後の係数
		const float lerpT = (easing_)(t);

		const Vector2 result = SoLib::Lerp(pos, position_, lerpT);

		return result;
	}

}

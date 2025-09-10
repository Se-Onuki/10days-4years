#include "FocusCamera.h"
#include "../LevelMapChip.h"
#include "Player.h"

namespace TD_10days {

	void FocusManager::Update(const float) {

		//const auto &lvMap = *pLevelMapChip_;


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

		const Vector2 targetPos = CalcTargetPoint();

		const Vector2 cameraPos = camera_->translation_.ToVec2();

		camera_->translation_.ToVec2() = SoLib::Lerp(targetPos, cameraPos, 0.9f);

		// カメラの更新処理
		camera_->UpdateMatrix();
	}

	Vector2 FocusCamera::CalcTargetPoint()
	{// プレイヤの位置からある程度位置を算出
		const Vector2 playerPos = pFocusEntity_->GetPosition();
		const Vector2 diff{ pFocusEntity_->GetVelocity().x * 0.25f,0.f };

		playerDiff_ = SoLib::Lerp(diff, playerDiff_, 0.5f);

		Vector2 targetPos = playerPos + playerDiff_;
		targetPos.x = std::clamp(targetPos.x, min_.x, max_.x);
		targetPos.y = std::clamp(targetPos.y, min_.y, max_.y);
		return targetPos;
	}

	void FocusCamera::CalcWindowSpan()
	{
		min_ = camera_->GetVisibleSize() / 2;
		max_ = Vector2{ static_cast<float>(x_), static_cast<float>(y_) } - (camera_->GetVisibleSize() / 2) - Vector2::one;
	}


}

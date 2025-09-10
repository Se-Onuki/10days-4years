#include "PlacementUI.h"

#include "../../../Engine/DirectBase/Input/Input.h"

void TD_10days::PlacementUI::Init(const Vector2& basePos)
{

	for (int i = 0; i < 4; ++i) {

		Arrow arrow;
		arrow.state_ = ArrowState::kNone;
		for (int j = 0; j < 3; ++j) {
			std::unique_ptr<Sprite> sprite = Sprite::Generate(TextureManager::Load("SetArrow.png"));
			sprite->SetPivot({ 0.5f, 0.5f });
			sprite->SetRotate(SoLib::Angle::Dig2Rad * (90.0f * i));
			sprite->SetScale(arrowSize_);
			sprite->SetTexOrigin(Vector2{ texSize_.x * j, 0.0f });
			sprite->SetTexDiff(Vector2{ texSize_.x, texSize_.y });
			Vector2 pos = basePos + Vector2{ 0.0f, uiOffset_ };
			switch (i) {
			case 0: pos.x += offset_ + 0.25f; // 右
				break;
			case 1: pos.y += offset_; // 上
				break;
			case 2: pos.x -= offset_ + 0.25f; // 左
				break;
			case 3: pos.y -= offset_; // 下
				break;

			}
			sprite->SetPosition(pos);
			arrow.sprites_.emplace_back(std::move(sprite));
		}
		arrows_.emplace_back(std::move(arrow));
	}

	text_ = Sprite::Generate(TextureManager::Load("SetFinishUI.png"));
	text_->SetInvertY(true);
	text_->SetPivot({ 0.5f, 0.5f });
	text_->SetPosition(basePos + Vector2{ 0.0f, uiOffset_ - 0.25f});
	text_->SetScale(texSize_);

	button_ = Sprite::Generate(TextureManager::Load("RBButtomUI.png"));
	button_->SetInvertY(true);
	button_->SetPivot({ 0.5f, 0.5f });
	button_->SetScale(buttonSize_);
	button_->SetPosition(basePos + Vector2{ 0.0f, uiOffset_ + 0.25f});
}

void TD_10days::PlacementUI::Update(float deltaTime)
{
	if(!isActive_) {
		return;
	}
	/*const auto input = SolEngine::Input::GetInstance();
	const auto dInput = input->GetDirectInput();*/
	if (easingState_ == EasingState::kAppearing) {
		elapsed_ += deltaTime / appearTime_;
		if (elapsed_ >= 1.0f) {
			elapsed_ = 1.0f;
			easingState_ = EasingState::kNone; // 完了
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				arrows_[i].sprites_[j]->SetScale(SoLib::Lerp(Vector2{0.0f, 0.0f}, arrowSize_, elapsed_));
				
			}
			
		}
		text_->SetScale(SoLib::Lerp(Vector2{ 0.0f, 0.0f }, textSize_, elapsed_));
		button_->SetScale(SoLib::Lerp(Vector2{ 0.0f, 0.0f }, buttonSize_, elapsed_));


	}
	else if (easingState_ == EasingState::kDisappearing) {
		elapsed_ += deltaTime / disappearTime_;
		if (elapsed_ >= 1.0f) {
			elapsed_ = 1.0f;
			easingState_ = EasingState::kNone; // 完了
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				arrows_[i].sprites_[j]->SetScale(SoLib::Lerp(Vector2{ 0.0f, 0.0f }, arrowSize_, elapsed_));
			}

		}
		text_->SetScale(SoLib::Lerp(textSize_, Vector2{ 0.0f, 0.0f }, elapsed_));
		button_->SetScale(SoLib::Lerp(buttonSize_, Vector2{ 0.0f, 0.0f }, elapsed_));
	}


	// 位置更新
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j) {
			Vector2 pos = basePos_ + Vector2{ 0.0f, uiOffset_ };
			switch (i) {
			case 0: pos.x += offset_ + 0.25f; // 右
				break;
			case 1: pos.y += offset_; // 上
				break;
			case 2: pos.x -= offset_ + 0.25f; // 左
				break;
			case 3: pos.y -= offset_; // 下
				break;

			}
			arrows_[i].sprites_[j]->SetPosition(pos);
		}

		if (arrows_[i].pressTimer > 0.0f) {
			arrows_[i].pressTimer -= deltaTime;
			if(arrows_[i].pressTimer <= 0.0f) {
				arrows_[i].pressTimer = 0.0f;
				arrows_[i].state_ = ArrowState::kNone;
			}
		}

	}

	const auto input = SolEngine::Input::GetInstance();
	const auto dInput = input->GetDirectInput();
	const auto* const xInput = input->GetXInput();

	// 次に設置する水の場所
	Vector2 nextDir = Vector2::zero;
	if (xInput->GetPreState()->stickL_.LengthSQ() <= 0.0625f) {
		nextDir = xInput->GetState()->stickL_;
		if (std::abs(nextDir.x) > std::abs(nextDir.y)) {
			nextDir.y = 0.f;
		}
		else {
			nextDir.x = 0.f;
		}
		nextDir = nextDir.Normalize();
	}




	// 入力に応じて値を加算する
	if (dInput->IsTrigger(DIK_D) or nextDir.x > 0.0f) {
		if (arrows_[0].state_ != ArrowState::kIncapable) {
			arrows_[0].state_ = ArrowState::kTrigger;
			arrows_[0].pressTimer = arrows_[0].pressDuration;
		}
		
	}
	if (dInput->IsTrigger(DIK_W) or nextDir.y > 0.0f) {
		if (arrows_[1].state_ != ArrowState::kIncapable) {
			arrows_[1].state_ = ArrowState::kTrigger;
			arrows_[1].pressTimer = arrows_[1].pressDuration;
		}
	}
	if (dInput->IsTrigger(DIK_A) or nextDir.x < 0.0f) {
		if (arrows_[2].state_ != ArrowState::kIncapable) {
			arrows_[2].state_ = ArrowState::kTrigger;
			arrows_[2].pressTimer = arrows_[2].pressDuration;
		}
	}
	if (dInput->IsTrigger(DIK_S) or nextDir.y < 0.0f) {
		if (arrows_[3].state_ != ArrowState::kIncapable) {
			arrows_[3].state_ = ArrowState::kTrigger;
			arrows_[3].pressTimer = arrows_[3].pressDuration;
		}
	}

	text_->SetPosition(basePos_ + Vector2{ 0.0f, uiOffset_ - 0.25f });

	button_->SetPosition(basePos_ + Vector2{ 0.0f, uiOffset_ + 0.25f });

}

void TD_10days::PlacementUI::Draw()
{
	if(!isActive_) {
		return;
	}
	for(int i = 0; i < 4; ++i) {

		if (not isPlaceableDir_[i]) {
			arrows_[i].state_ = ArrowState::kIncapable;
		}
		else if (arrows_[i].state_ == ArrowState::kIncapable) {
			arrows_[i].state_ = ArrowState::kNone;
		}


		arrows_[i].sprites_[static_cast<int>(arrows_[i].state_)]->Draw();
		
	}
	text_->Draw();
	button_->Draw();
}



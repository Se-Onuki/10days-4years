#include "Player.h"

namespace TD_10days {

	void Player::Init() {
		sprite_ = Sprite::Generate(TextureManager::Load("uvChecker.png"));
		sprite_->SetScale(size_);
		sprite_->SetInvertY(true);

	}

	void Player::Update([[maybe_unused]] const float deltaTime) {

		MoveUpdate(deltaTime);

		CalcSprite();
	}

	void Player::Draw() const {
		sprite_->Draw();
	}

	void Player::CalcSprite() {
		// 座標合わせ
		sprite_->SetPosition(position_);
	}

	void Player::MoveUpdate(float deltaTime) {
		velocity_ += acceleration_ * deltaTime;
		position_ += velocity_ * deltaTime;
		acceleration_ = Vector2::zero;
	}

}

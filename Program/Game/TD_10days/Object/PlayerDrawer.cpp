#include "PlayerDrawer.h"

#include "Player.h"

namespace TD_10days {


	// PlayerDrawer::Init() 内の該当箇所を修正
	void PlayerDrawer::Init(Player *player) {
		// もしデータがないなら生成する
		for (auto &ptr : parametors_) {
			if (not ptr) {
				ptr = std::make_unique<PlayerDrawParametor>();
			}
		}

		GetParametor(PlayerDrawState::kIdle) = PlayerDrawParametor{
			.vTransitionTime_ = 1.f,
			.vTextureFrames_ = 4u,
			.texture_ = TextureManager::Load("TD_10days/Player/PlayerIdle.png")
		};

		GetParametor(PlayerDrawState::kSwim) = PlayerDrawParametor{
			.vTransitionTime_ = 1.f,
			.vTextureFrames_ = 7u,
			.texture_ = TextureManager::Load("TD_10days/Player/PlayerSwim.png")
		};

		GetParametor(PlayerDrawState::kWalk) = PlayerDrawParametor{
			.vTransitionTime_ = 0.5f,
			.vTextureFrames_ = 5u,
			.texture_ = TextureManager::Load("TD_10days/Player/PlayerWalk.png")
		};

		GetParametor(PlayerDrawState::kVomit) = PlayerDrawParametor{
			.vTransitionTime_ = 0.25f,
			.vTextureFrames_ = 4u,
			.texture_ = TextureManager::Load("TD_10days/Player/PlayerVomit.png")
		};

		GetParametor(PlayerDrawState::kDead) = PlayerDrawParametor{
			.vTransitionTime_ = 1.f,
			.vTextureFrames_ = 8u,
			.texture_ = TextureManager::Load("TD_10days/Player/PlayerDeath.png"),
			.vIsMultiPlay_ = false,
		};

		pPlayer_ = player;

		sprite_ = Sprite::Generate();
		sprite_->SetInvertY(true);

		sprite_->SetPivot(Vector2::one * 0.5f);

		swimSETimer_ = std::make_unique<SoLib::DeltaTimer>();
		swimSETimer_->Clear();
	}

	void PlayerDrawer::Update(const float deltaTime) {
		// 時間の増加
		stateTime_ += deltaTime;



		const auto sample = TransitionSample(*pPlayer_);
		waterCount_ = pPlayer_->pWater_->GetWaterCount();

		const auto &parametor = GetParametor(sample);

		if (playerState_ != sample) {
			playerState_ = sample;
			stateTime_ = 0.f;

			sprite_->SetTextureHaundle(parametor.texture_.index_);
			sprite_->SetTexDiff(Vector2{ 200.f,200.f });
		}

		// 移動しているなら

		const float t = parametor.vIsMultiPlay_ ? stateTime_ / parametor.vTransitionTime_ : std::clamp(stateTime_ / parametor.vTransitionTime_, 0.f, 1.f);
		const float uvX = std::floor(parametor.vTextureFrames_ * t);
		constexpr float pixelSize = 200.f;

		if (pPlayer_->velocity_.x != 0.f) {

			// 左向きの移動であればtrue､そうでないならfalse;
			isLeftFacing_ = pPlayer_->velocity_.x < 0.f;
		}

		swimSETimer_->Update(deltaTime);
		//泳いでいてアニメーションが変化したら
		if (sample == PlayerDrawState::kSwim) {
			if (not swimSETimer_->IsActive() and (pPlayer_->velocity_.x != 0.f or pPlayer_->velocity_.y != 0.f)) {
				pPlayer_->GetWaterSwimSE().Play(false, pPlayer_->GetSEValume());
				swimSETimer_->Clear();
				swimSETimer_->Start(0.7f);
			}

		}

		sprite_->SetTexOrigin({ uvX * pixelSize ,0 });
		sprite_->SetPosition(pPlayer_->GetPosition() - Vector2::up * 0.1f);

		sprite_->SetInvertX(isLeftFacing_);

	}

	void PlayerDrawer::Draw() const {
		sprite_->Draw();
	}

	PlayerDrawParametor &PlayerDrawer::GetParametor(PlayerDrawState state) {
		return *parametors_.at(static_cast<size_t>(state));
	}

	const PlayerDrawParametor &PlayerDrawer::GetParametor(PlayerDrawState state) const {
		return *parametors_.at(static_cast<size_t>(state));
	}

	PlayerDrawState PlayerDrawer::TransitionSample(const Player &player) const
	{
		const auto &playerStateName = player.playerState_->GetStateName();
		if (playerStateName == "PlayerDead") {
			return PlayerDrawState::kDead;
		}

		// もし設置状態なら設置状態にする
		if (playerStateName == "PlayerPlacement") {
			// 吐き出す処理が完了してない場合は吐き出し状態にする
			if (playerState_ == PlayerDrawState::kVomit and (stateTime_ / GetParametor(playerState_).vTransitionTime_) <= 1.f) {
				return PlayerDrawState::kVomit;
			}
		}
		// 水の中は泳ぎ
		if (player.IsInWater()) {
			// 水の個数が増加した瞬間なら､吐き出しにする
			if (pPlayer_->pWater_->GetWaterCount() > waterCount_) {
				return PlayerDrawState::kVomit;
			}

			return PlayerDrawState::kSwim;
		}


		// 歩いてたら歩き
		if (player.velocity_.x != 0.f) {
			return PlayerDrawState::kWalk;
		}
		// 全てに一致しないなら歩き
		return PlayerDrawState::kIdle;
	}

	void PlayerDrawer::CalcSpriteData() {

	}

}
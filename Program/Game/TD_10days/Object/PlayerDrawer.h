#pragma once
#include <memory>
#include "DirectBase/2D/Sprite.h"
#include "Utils/Containers/Singleton.h"

namespace TD_10days {

	class Player;

	enum class PlayerDrawState : uint32_t {
		kIdle,	// 静止状態
		kWalk,	// 歩行
		kSwim,	// 泳ぐ
		kVomit,	// 吐き出す
		kDead,	// 死亡処理

		CountElements // 要素数
	};

	struct PlayerDrawParametor {

		// 次の画像に遷移する時間
		VariantItem<"一周にかかる時間", float> vTransitionTime_;
		VariantItem<"画像数", uint32_t> vTextureFrames_;

		// 使用する画像
		TextureHandle texture_;
	};



	class PlayerDrawer
	{
	public:
		PlayerDrawer() = default;
		~PlayerDrawer() = default;

		void Init(Player *player);

		void Update(const float deltaTime);

		void Draw() const;

		PlayerDrawParametor &GetParametor(PlayerDrawState state);
		const PlayerDrawParametor &GetParametor(PlayerDrawState state) const;

	private:

		PlayerDrawState TransitionSample(const Player &player) const;

		void CalcSpriteData();

	private:

		std::unique_ptr<Sprite> sprite_;
		Player *pPlayer_;
		size_t waterCount_ = 0;

		std::array<std::unique_ptr<PlayerDrawParametor>, static_cast<size_t>(PlayerDrawState::CountElements)> parametors_;

		PlayerDrawState playerState_ = PlayerDrawState::CountElements;
		float stateTime_;

		bool isLeftFacing_ = false;
	};
}
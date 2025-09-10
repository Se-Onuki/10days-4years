#pragma once
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../../../Engine/DirectBase/Render/Camera.h"
#include<DirectBase/Base/Audio.h>
#include "../LevelMapChip.h"
#include "Water.h"
#include <DirectBase/File/GlobalVariables.h>
#include "../Game/UI/PlacementUI.h"
#include "../Game/TD_10days/Particle/ParticleManager.h"
#include "../Game/TD_10days/CountUI.h"
#include "PlayerDrawer.h"

namespace TD_10days {

	class Player;

	class IPlayerState {
	public:
		IPlayerState() = default;
		IPlayerState(Player *const pPlayer) : pPlayer_(pPlayer) {}
		virtual ~IPlayerState() = default;

		virtual void InputFunc() = 0;
		virtual const std::string &GetStateName() const = 0;
		virtual void OnEnter() {}
		virtual void OnExit() {}

		Player *GetPlayer() { return pPlayer_; }

	private:
		Player *pPlayer_ = nullptr;
	};

	class PlayerMovement : public IPlayerState {
	public:
		PlayerMovement() = default;
		PlayerMovement(Player *const pPlayer) : IPlayerState(pPlayer) {}
		~PlayerMovement() = default;

		void InputFunc() override;
		const std::string &GetStateName() const override { return kStateName_; }

		void OnEnter() override;
		void OnExit() override;

	private:

		inline static const std::string kStateName_ = "PlayerMovement";

	};

	class PlayerPlacement : public IPlayerState {
	public:
		PlayerPlacement() = default;
		PlayerPlacement(Player *const pPlayer) : IPlayerState(pPlayer) {}
		~PlayerPlacement() = default;

		void InputFunc() override;
		const std::string &GetStateName() const override { return kStateName_; }

		void OnEnter() override;
		void OnExit() override;
	private:

		Vector2 InputPlaceAble();

	private:
		bool isInputAble_{};
	private:

		inline static const std::string kStateName_ = "PlayerPlacement";

	};

	class PlayerDead : public IPlayerState {
	public:
		PlayerDead() = default;
		PlayerDead(Player *const pPlayer) : IPlayerState(pPlayer) {}
		~PlayerDead() = default;

		void InputFunc() override;
		const std::string &GetStateName() const override { return kStateName_; }

		void OnEnter() override;
		void OnExit() override;

	private:

		inline static const std::string kStateName_ = "PlayerDead";

	};

	class Player {
		friend IPlayerState;
		friend PlayerMovement;
		friend PlayerPlacement;
		friend PlayerDead;
		friend PlayerDrawer;
	public:

		Player() = default;

		void Init();
		void PreUpdate(float deltaTime);
		void Update(float deltaTime);

		void Draw() const;

		void DrawUI() const;

		void SetPosition(const Vector2 &position) { position_ = position; }

		void InputFunc();

		void SetHitBox(const LevelMapChip::LevelMapChipHitBox *const pHitBox) { pHitBox_ = pHitBox; }
		void SetWaterHitBox(const LevelMapChip::LevelMapChipHitBox *const pHitBox) { pWaterHitBox_ = pHitBox; }

		Vector2 &GetPosition() { return position_; }

		std::array<Vector2, 4u> GetVertex() const;

		void SetWater(Water *water) { pWater_ = water; }

		void Load();

		void Save() const;

		PlacementUI* GetPlacementUI() { return placementUI_.get(); }

		float GetSEValume()const {return SEValume_;	}

		SolEngine::Audio::SoundHandle GetWaterSettingSE() const { return waterSettingSE_; }
		SolEngine::Audio::SoundHandle GetWaterInOutSE() const { return waterInOutSE_; }
		SolEngine::Audio::SoundHandle GetWaterSwimSE() const { return waterSwimSE_; }
		void SetParticleManager(ParticleManager *particleManager) { particleManager_ = particleManager; }

		

	private:

		void SetPosInStage();

	private:

		inline static const std::string kPlayerGroup_ = "PlayerParameter";


		/// @brief 画像の更新処理
		void CalcSprite();

		/// @brief 移動ベクトルに対する移動可能な進行度を計算する
		/// @param[in] velocity 移動ベクトル
		/// @return (進行度, 衝突した法線ベクトル)
		std::tuple<float, Vector3> CalcMoveProgress(const Vector2 &velocity);

		/// @brief 移動処理
		/// @param[in] deltaTime 前フレームからの経過時間
		void MoveUpdate(float deltaTime);

		/// @brief 水の中にいるか
		/// @return 水の中にいるならTrue
		bool IsInWater() const;

	private:

		Water *pWater_ = nullptr;
		std::unique_ptr<IPlayerState> playerState_;
		std::unique_ptr<IPlayerState> nextState_;

		std::string spriteName_ = "TD_10days/Player/Player.png";

		// 描画スプライト
		std::unique_ptr<Sprite> sprite_;
		// 当たり判定
		const LevelMapChip::LevelMapChipHitBox *pHitBox_ = nullptr;
		const LevelMapChip::LevelMapChipHitBox *pWaterHitBox_ = nullptr;
		// 水ブロック操作UI
		std::unique_ptr<PlacementUI> placementUI_;
		ParticleManager *particleManager_;

		std::unique_ptr<CountUI> countUI_;
		//水を置いたときの音
		SolEngine::Audio::SoundHandle waterSettingSE_;
		//水を出入り(水と当たった)時の音
		SolEngine::Audio::SoundHandle waterInOutSE_;
		//水を泳いでいる時の音
		SolEngine::Audio::SoundHandle waterSwimSE_;

		float SEValume_ = 0.5f;

		// 座標
		Vector2 position_{};
		// 移動量
		Vector2 velocity_{};
		// 加速度
		Vector2 acceleration_{};

		Vector2 size_ = Vector2::one * 0.8f;

		Vector2 gravity_ = Vector2{ 0.f, -9.8f };

		VItem(float, AirSpeed, _) { 5.f };
		VItem(float, WaterSpeed, _) { 3.5f };
		VItem(float, AirGravity, _) { -9.8f };
		VItem(float, WaterGravity, _) { -4.9f };
		VItem(float, WaterJumpPower, _) { 2.5f };
		VItem(float, WaterLifeTime, _) { 3.f };

		bool isGround_ = false;

		bool wasInWater_ = false;
	};
}
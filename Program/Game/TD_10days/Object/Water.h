#pragma once
#include <cstdint>
#include <list>
#include<DirectBase/Base/Audio.h>
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../LevelMapChip.h"
#include "../Game/TD_10days/WaterEffect.h"
namespace TD_10days {

	class Water {
	public:

		class ChainWater {
		public:

			struct ChainWaterData {

				/// @brief 画像データ
				//std::unique_ptr<Sprite> sprite_;

				/// @brief 位置
				Vector2 position_{};

				ChainWaterData *next_ = nullptr;


				static std::unique_ptr<ChainWaterData> Generate(const uint32_t color);
				/// @brief 位置の設定
				/// @param[in] position 位置
				void SetPosition(const Vector2 &position);
			};

			ChainWater() = default;
			~ChainWater() = default;

			void Init(const Vector2 &size, const uint32_t color);

			void Update(float deltaTime);
			void Draw() const;

			void CalcUpdate();

			ChainWaterData *CreateChain(const Vector2 &position);
			void MoveDirection(const Vector2 &direction);

			/// @brief 水の座標を取得する
			/// @return 水の座標のリスト
			std::unordered_set<Vector2> GetPositionList() const;

			/// @brief 水のリストを破棄する
			void Clear();

			size_t GetWaterCount() const { return chainWaterList_.size(); }

		private:


			std::list<std::unique_ptr<ChainWaterData>> chainWaterList_;

			Vector2 size_{};
			uint32_t color_ = 0x0000FF55;

		};

		Water() = default;
		~Water() = default;
		void Init(const Vector2 &position, const Vector2 &size, const uint32_t color);
		void Update(float deltaTime);
		void Draw() const;

		/// @brief 水の破棄処理
		void DeleteWater();

		/// @brief 水の設置処理
		/// @param[in] direction 設置したい方向性
		void PlacementWater(const Vector2 &direction);

		/// @brief 水が配置できるか
		/// @param[in] hitBox 当たり判定
		/// @param[in] direction 移動方向
		/// @return 設置可能ならtrue
		bool IsPlaceAble(const TD_10days::LevelMapChip::LevelMapChipHitBox *hitBox, const Vector2 &direction) const;

		/// @brief 動作開始
		void Activate(float maxTime);

		const std::unordered_set<Vector2> GetWaterPosition() const;

		WaterParticleManager *GetWaterPartilceManager() { return waterParticleManager_; }
		void SetWaterParticleManager(WaterParticleManager *manager) { waterParticleManager_ = manager; }

		/// @brief タイマーの時間を返す
		/// @return nulloptの場合は､タイマーが動作してない
		std::optional<float> GetWaterTime() const;

		/// @brief 水の個数の取得
		/// @return 水の量
		size_t GetWaterCount() const { return chainWater_ ? chainWater_->GetWaterCount() : 0u; }

	private:
		std::unique_ptr<ChainWater> chainWater_;
		Vector2 position_{};
		Vector2 size_{};
		uint32_t color_ = 0x0000FF55;
		float lifeTime_ = 0.f;

		bool isActive_ = false;

		//水を置いたときの音
		SolEngine::Audio::SoundHandle waterFallSE_;

		WaterParticleManager *waterParticleManager_ = nullptr;
	};
}
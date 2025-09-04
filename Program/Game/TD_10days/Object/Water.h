#pragma once
#include <cstdint>
#include <list>
#include "../../../Engine/Utils/Math/Math.hpp"
#include "../../../Engine/DirectBase/2D/Sprite.h"
#include "../LevelMapChip.h"

namespace TD_10days {

	class Water {
	public:

		class ChainWater {
		public:

			struct ChainWaterData {

				/// @brief 画像データ
				std::unique_ptr<Sprite> sprite_;

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

			std::vector<Vector2> GetPositionList() const;

			void Clear();

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

		void DeleteWater();

		void PlacementWater(const Vector2 &direction);

		bool IsPlaceAble(const TD_10days::LevelMapChip::LevelMapChipHitBox *hitBox, const Vector2& direction) const;

		void Activate();

		const std::vector<Vector2> GetWaterPosition() const;
	private:
		std::unique_ptr<ChainWater> chainWater_;
		Vector2 position_{};
		Vector2 size_{};
		uint32_t color_ = 0x0000FF55;
		float maxLifeTime_ = 5.f;
		float lifeTime_ = 0.f;

		bool isActive_ = false;
	};
}
#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>

#include "../../Engine/DirectBase/Render/Camera.h"			// カメラの情報
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャの情報

#include "../../Engine/DirectBase/File/VariantItem.h"		// 外部編集可能な変数の定義
#include "../../Engine/Utils/IO/CSV.h"
#include "../../Engine/DirectBase/2D/Sprite.h"

namespace TD_10days {
	class LevelMapChip {
	public:

		enum class MapChipType : uint32_t {
			kEmpty,	// 何もない
			kWall,		// 壁
			kTile,	// 床内部
			kFloor,	// プレイヤーが触れる床
			kWater,	// 設置するタイプの水
			kStart,	// スタート位置
			kGoal,	// ゴール位置
			CountElements // 要素数
		};

		using MapChip = MapChipType;

		class MapChipData {
		public:
			explicit operator const MapChip() const;
			MapChipData() = default;
			MapChipData(const TextureHandle texture) : textureHandle_(texture) {}


			/// @brief テクスチャハンドルを取得する
			/// @return テクスチャハンドル
			const TextureHandle& GetTextureHandle() const { return textureHandle_; }

		private:
			TextureHandle textureHandle_;

		};

		class LevelMapChipHitBox {
		public:
			LevelMapChipHitBox() = default;
			~LevelMapChipHitBox() = default;
			friend class LevelMapChip;

			bool at(const size_t y, const size_t x) const;

			LevelMapChipHitBox Clip(const Vector2& origin, const Vector2& diff) const;

			size_t GetY() const { return y_; }
			size_t GetX() const { return x_; }

		private:
			std::vector<bool> hitBoxData_;
			size_t y_{}, x_{};
		};

		LevelMapChip() = default;
		LevelMapChip(const LevelMapChip&) = default;
		LevelMapChip(const uint32_t y, const uint32_t x);
		LevelMapChip(const SoLib::IO::CSV& csv);

		void Init(const uint32_t y, const uint32_t x);
		void Init(const SoLib::IO::CSV& csv);

		/// @brief インデックス演算子
		/// @param[in] index 行のインデックス
		/// @return 行のマップチップの配列
		std::span<MapChip> operator[](const uint32_t index);
		std::span<const MapChip> operator[](const uint32_t index) const;

		/// @brief マップチップのサイズを取得する
		/// @return マップチップのサイズ(y, x)
		auto GetSize() const { return std::make_pair(y_, x_); }

		/// @brief マップチップに対応するデータを取得する
		/// @return マップチップに対応するデータの配列
		std::span<const MapChipData> GetMapChipData() const { return mapChipData_; }

		std::span<const MapChip> GetMapChips() const { return mapChips_; }

		void SetMapChipData(const std::vector<MapChipData>& mapChipData) { mapChipData_ = mapChipData; }

		LevelMapChipHitBox CreateHitBox() const;

		void Resize(uint32_t y, uint32_t x);

	private:
		/// @brief マップチップの配列
		std::vector<MapChip> mapChips_;
		/// @brief マップチップに対応するデータの配列
		std::vector<MapChipData> mapChipData_;

		/// @brief マップチップの縦横の数
		uint32_t y_{}, x_{};
	};

	class LevelMapChipRenderer {
	public:

		LevelMapChipRenderer() = default;
		LevelMapChipRenderer(const LevelMapChipRenderer&) = delete;
		void Init(const LevelMapChip& levelMapChip);
		void Draw();

	private:

		std::list<std::unique_ptr<Sprite>> spriteList_;
		const LevelMapChip* pLevelMapChip_;
		/// @brief マップチップの位置を計算する
		Vector2 CalcMapChipPosition(const uint32_t y, const uint32_t x) const;


		VItem(float, MapChipScale, _) = 1.f;

	};
}
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
			kEmpty,		// 何もない
			kWall,		// 壁
			kTile,		// 床内部
			kFloor,		// プレイヤーが触れる床
			kWater,		// 設置するタイプの水
			kStart,		// スタート位置
			kGoal,		// ゴール位置
			kNeedle,	// 針
			kMesh,		// 金網
			kMoveUI,		// 移動UI
			kSwimUI,		// 泳ぎUI
			kWaterSetUI,	// 水設置UI
			kGoalBord,		// ゴール上の看板
			CountElements // 要素数
		};

		using MapChip = MapChipType;

		class MapChipData {
		public:
			// explicit operator const MapChip() const;

			MapChipData() = default;
			MapChipData(const TextureHandle texture) : textureHandle_(texture) {}
			MapChipData(const TextureHandle texture, bool isHasCollision) : textureHandle_(texture), isHasCollision_(isHasCollision), isWaterCollision_(isHasCollision) {}
			MapChipData(const TextureHandle texture, bool isHasCollision, bool isWaterCollision) : textureHandle_(texture), isHasCollision_(isHasCollision), isWaterCollision_(isWaterCollision) {}
			MapChipData(bool isHasCollision) : isHasCollision_(isHasCollision), isWaterCollision_(isHasCollision) {}
			//MapChipData(const TextureHandle texture, bool isVisible, const std::function<void(const Vector2 &)> &callback) : textureHandle_(texture), callback_(callback), isVisible_(isVisible) {}


			/// @brief テクスチャハンドルを取得する
			/// @return テクスチャハンドル
			const TextureHandle &GetTextureHandle() const { return textureHandle_; }

			/// @brief 当たり判定があるか
			/// @return 当たり判定があるならtrue
			bool IsHasCollision() const { return isHasCollision_; }

			/// @brief 水に対する当たり判定があるかどうか
			/// @return 水に対して当たるのならtrue
			bool IsHasWaterCollision() const { return isWaterCollision_; }

			// 描画スケール
			void SetDrawScale(const Vector2 &scale) { scale_ = scale; }
			const Vector2 &GetDrawScale() const { return scale_; }

		private:

			// 描画時のスケール
			Vector2 scale_ = Vector2::one;
			/// @brief テクスチャハンドル
			TextureHandle textureHandle_{};
			// 当たり判定があるかどうか
			bool isHasCollision_ = true;
			// 水に対する当たり判定があるかどうか
			bool isWaterCollision_ = true;
		};

		class LevelMapChipHitBox {
		public:
			LevelMapChipHitBox() = default;
			~LevelMapChipHitBox() = default;
			friend class LevelMapChip;

			bool at(const size_t y, const size_t x) const;

			LevelMapChipHitBox Clip(const Vector2 &origin, const Vector2 &diff) const;

			size_t GetY() const { return y_; }
			size_t GetX() const { return x_; }

		private:
			std::vector<bool> hitBoxData_;
			size_t y_{}, x_{};
		};

		LevelMapChip() = default;
		LevelMapChip(const LevelMapChip &) = default;
		LevelMapChip(const uint32_t y, const uint32_t x);
		LevelMapChip(const SoLib::IO::CSV &csv);

		void Init(const uint32_t y, const uint32_t x);
		void Init(const SoLib::IO::CSV &csv);

		/// @brief 特殊効果を持つ場所を探す
		void FindActionChips();

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
		std::span<MapChipData> GetMapChipData() { return mapChipData_; }
		/// @brief マップチップに対応するデータを取得する
		/// @return マップチップに対応するデータの配列
		std::span<const MapChipData> GetMapChipData() const { return mapChipData_; }

		std::span<const MapChip> GetMapChips() const { return mapChips_; }

		void SetMapChipData(const std::vector<MapChipData> &mapChipData) { mapChipData_ = mapChipData; }
		void AppendMapChipData(const MapChipData &mapChipData) { mapChipData_.emplace_back(mapChipData); }

		/// @brief ヒットボックスを作成します
		void CreateHitBox();

		const LevelMapChipHitBox *GetPlayerHitBox() const { return &playerHitBox_; }
		const LevelMapChipHitBox *GetWaterHitBox() const { return &waterHitBox_; }

		/// @brief サイズの調整
		/// @param[in] y 更新後の高さ
		/// @param[in] x 更新後の幅
		void Resize(uint32_t y, uint32_t x);

		Vector2 GetStartPosition() const;
		const std::unordered_set<Vector2> &GetGoalPosition() const;
		const std::unordered_set<Vector2> &GetNeedlePosition() const;

	private:


		/// @brief ヒットボックスを作成し、返します。
		/// @return 新しく作成された LevelMapChipHitBox
		std::unique_ptr<LevelMapChipHitBox> CreateHitBox(const std::function<bool(const MapChip &)> &checkFunc) const;
		/// @brief マップチップの配列
		std::vector<MapChip> mapChips_;
		/// @brief マップチップに対応するデータの配列
		std::vector<MapChipData> mapChipData_;

		LevelMapChipHitBox playerHitBox_;
		LevelMapChipHitBox waterHitBox_;

		Vector2 startPos_{};
		std::unordered_set<Vector2> goalPosList_{};
		std::unordered_set<Vector2> needlePosList_{};

		/// @brief マップチップの縦横の数
		uint32_t y_{}, x_{};
	};

	class LevelMapChipRenderer {
	public:

		LevelMapChipRenderer() = default;
		LevelMapChipRenderer(const LevelMapChipRenderer &) = delete;
		void Init(const LevelMapChip *levelMapChip);
		void CalcSpriteData();
		void Draw();

		/// @brief 
		/// @param[in] pos 
		/// @return 
		Vector2 StageToDrawMap(Vector2 pos);

	private:

		std::vector<std::unique_ptr<Sprite>> spriteList_;
		const LevelMapChip *pLevelMapChip_ = nullptr;
		/// @brief マップチップの位置を計算する
		Vector2 CalcMapChipPosition(const uint32_t y, const uint32_t x) const;


		VItem(float, MapChipScale, _) = 1.f;

	};
}
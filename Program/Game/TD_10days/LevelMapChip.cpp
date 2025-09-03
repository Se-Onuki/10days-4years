#include "LevelMapChip.h"
#include <utility>

namespace TD_10days {
	// 通常のコンストラクタ
	LevelMapChip::LevelMapChip(const uint32_t y, const uint32_t x) : y_(y), x_(x), mapChips_(y *x, static_cast<MapChip>(0)) {}

	// CSVからのコンストラクタ
	LevelMapChip::LevelMapChip(const SoLib::IO::CSV &csv)
	{
		// 縦横のデータを取得
		y_ = static_cast<uint32_t>(csv.GetWidth());
		x_ = static_cast<uint32_t>(csv.GetHeight());

		mapChips_.resize(y_ * x_, static_cast<MapChip>(0));

		// CSVのデータをマップチップに変換して格納
		for (uint32_t yi = 0; yi < y_; ++yi) {
			for (uint32_t xi = 0; xi < x_; ++xi) {
				const uint32_t value = std::clamp(std::stoi(csv[yi][xi]), 0, static_cast<int>(MapChipType::CountElements) - 1);
				mapChips_[xi + yi * x_] = static_cast<MapChip>(value);
			}
		}

	}

	void LevelMapChip::Init(const uint32_t y, const uint32_t x)
	{
		x_ = x; y_ = y; mapChips_.resize(y * x, static_cast<MapChip>(0));
	}

	void LevelMapChip::Init(const SoLib::IO::CSV &csv)
	{
		// 縦横のデータを取得
		y_ = static_cast<uint32_t>(csv.GetWidth());
		x_ = static_cast<uint32_t>(csv.GetHeight());

		mapChips_.resize(y_ * x_, static_cast<MapChip>(0));

		// CSVのデータをマップチップに変換して格納
		for (uint32_t yi = 0; yi < y_; ++yi) {
			for (uint32_t xi = 0; xi < x_; ++xi) {
				const uint32_t value = std::clamp(std::stoi(csv[yi][xi]), 0, static_cast<int>(MapChipType::CountElements) - 1);
				mapChips_[xi + yi * x_] = static_cast<MapChip>(value);
			}
		}
	}

	std::span<LevelMapChip::MapChip> LevelMapChip::operator[](const uint32_t index) {
		return std::span<MapChip>{ &mapChips_[index * x_], x_ };
	}

	std::span<const LevelMapChip::MapChip> LevelMapChip::operator[](const uint32_t index) const {
		return std::span<const MapChip>{ &mapChips_[index * x_], x_ };
	}

	void LevelMapChipRenderer::Init(const LevelMapChip &levelMapChip) {
		pLevelMapChip_ = &levelMapChip;
	}

	void LevelMapChipRenderer::Draw() {
		spriteList_.clear();
		// マップチップの情報の取得
		const auto &mapChips = pLevelMapChip_->GetMapChipData();

		// マップチップのサイズの取得
		const auto [mapChipY, mapChipX] = pLevelMapChip_->GetSize();

		///
		/// マップチップの描画
		///

		// 描画リスト
		std::unordered_map<size_t, std::vector<Vector2>> drawList;

		// マップチップのイテレータ
		auto mapChipItr = pLevelMapChip_->GetMapChips().begin();
		for (uint32_t y = 0; y < mapChipY; ++y) {
			for (uint32_t x = 0; x < mapChipX; ++x) {
				// マップチップの種類
				const auto &mapChipType = *(mapChipItr++);
				// マップチップのデータ
				//const auto &mapChipData = mapChips[static_cast<size_t>(mapChipType)];
				// マップチップの位置
				const auto mapChipPosition = CalcMapChipPosition(y, x);

				if (mapChipType == LevelMapChip::MapChip::kEmpty) {
					continue;
				}
				// データの追加
				drawList[static_cast<size_t>(mapChipType)].emplace_back(mapChipPosition);
			}
		}

		// 描画テーブルへの追加
		for (const auto &[index, positions] : drawList) {
			// マップチップのテクスチャハンドル
			const TextureHandle textureHandle = mapChips[index].GetTextureHandle();
			// 座標
			for (const auto &position : positions) {
				// スプライトの生成
				auto sprite = spriteList_.emplace_back(Sprite::Generate(textureHandle.index_, position, Vector2::one * vMapChipScale_)).get();
				// スプライトの設定
				sprite->SetPivot(Vector2::one / 2);	// 中心に設定
				sprite->SetInvertY(true);			// UVのY軸反転
			}
		}

		// 全てに対して描画を実行
		for (const auto &sprite : spriteList_) {
			sprite->Draw();
		}

	}
	Vector2 LevelMapChipRenderer::CalcMapChipPosition(const uint32_t y, const uint32_t x) const
	{
		return Vector2(x * vMapChipScale_, y * vMapChipScale_);
	}
}
#include "LevelMapChip.h"
#include <utility>

namespace TD_10days {

	bool LevelMapChip::LevelMapChipHitBox::at(const size_t y, const size_t x) const {
		if (y >= y_ || x >= x_) {
			return false;
		}
		return hitBoxData_[x + y * x_];
	}
	LevelMapChip::LevelMapChipHitBox LevelMapChip::LevelMapChipHitBox::Clip(const Vector2 &origin, const Vector2 &diff) const
	{
		if (origin.x >= x_ || origin.y >= y_) {
			return LevelMapChipHitBox();
		}
		if (diff.x <= 0.f || diff.y <= 0.f) {
			return LevelMapChipHitBox();
		}

		LevelMapChipHitBox result;

		result.hitBoxData_ = std::vector<bool>(static_cast<size_t>(diff.x) * static_cast<size_t>(diff.y), false);
		result.y_ = static_cast<size_t>(diff.y);
		result.x_ = static_cast<size_t>(diff.x);


		return result;
	}
	// 通常のコンストラクタ
	LevelMapChip::LevelMapChip(const uint32_t y, const uint32_t x) : y_(y), x_(x), mapChips_(y *x, static_cast<MapChip>(0)) {}

	// CSVからのコンストラクタ
	LevelMapChip::LevelMapChip(const SoLib::IO::CSV &csv)
	{
		Init(csv);
	}

	void LevelMapChip::Init(const uint32_t y, const uint32_t x)
	{
		x_ = x; y_ = y; mapChips_.resize(y * x, static_cast<MapChip>(0));
	}

	void LevelMapChip::Init(const SoLib::IO::CSV &csv)
	{
		// 縦横のデータを取得
		x_ = static_cast<uint32_t>(csv.GetWidth());
		y_ = static_cast<uint32_t>(csv.GetHeight());

		mapChips_.resize(y_ * x_, static_cast<MapChip>(0));

		// CSVのデータをマップチップに変換して格納
		for (uint32_t yi = 0; yi < y_; ++yi) {
			for (uint32_t xi = 0; xi < x_; ++xi) {
				const uint32_t value = std::clamp(std::stoi(csv[yi][xi]), 0, static_cast<int>(MapChipType::CountElements) - 1);
				mapChips_[xi + yi * x_] = static_cast<MapChip>(value);
			}
		}
	}

	void LevelMapChip::FindActionChips() {
		goalPosList_.clear();
		needlePosList_.clear();

		for (uint32_t yi = 0; yi < y_; ++yi) {
			const auto &line = (*this)[yi];
			for (uint32_t xi = 0; xi < x_; ++xi) {
				const auto chip = line[xi];

				switch (chip) {
				case MapChip::kStart: {
					startPos_ = Vector2{ static_cast<float>(xi), static_cast<float>(yi) };
					break;
				}
				case MapChip::kGoal: {
					goalPosList_.insert(Vector2{ static_cast<float>(xi), static_cast<float>(yi) });
					break;
				}
				case MapChip::kNeedle: {
					needlePosList_.insert(Vector2{ static_cast<float>(xi), static_cast<float>(yi) });
					break;
				}
				case MapChip::kFocusPoint: {
					focusPoints_[Vector2{ static_cast<float>(xi), static_cast<float>(yi) }];
					break;
				}
				default: {
					break;
				}
				}
			}
		}
	}

	std::span<LevelMapChip::MapChip> LevelMapChip::operator[](const uint32_t index) {
		return std::span<MapChip>{ &mapChips_[index * x_], x_ };
	}

	std::span<const LevelMapChip::MapChip> LevelMapChip::operator[](const uint32_t index) const {
		return std::span<const MapChip>{ &mapChips_[index * x_], x_ };
	}

	void LevelMapChip::CreateHitBox()
	{
		playerHitBox_ = *CreateHitBox([this](const MapChip &chip) {
			// 当たり判定のフラグを持っているならtrue
			return mapChipData_[static_cast<uint32_t>(chip)].IsHasCollision();
			});
		waterHitBox_ = *CreateHitBox([this](const MapChip &chip) {
			// 当たり判定のフラグを持っているならtrue
			return mapChipData_[static_cast<uint32_t>(chip)].IsHasWaterCollision();
			});

	}

	void LevelMapChip::Resize(uint32_t y, uint32_t x)
	{
		std::vector<MapChip> newData(y * x, MapChip::kEmpty);
		const uint32_t minX = (std::min)(x, x_);
		const uint32_t minY = (std::min)(y, y_);

		// すべての行でコピーを実行する
		for (uint32_t yi = 0; yi < minY; ++yi) {
			const MapChip *src = &mapChips_[yi * x_];  // 元の行の先頭
			MapChip *dst = &newData[yi * x];           // 新しい行の先頭
			std::copy_n(src, minX, dst);
		}

		// データを受け渡す
		y_ = y;
		x_ = x;
		mapChips_ = std::move(newData);
	}

	Vector2 LevelMapChip::GetStartPosition() const
	{
		return startPos_;
	}

	const std::unordered_set<Vector2> &LevelMapChip::GetGoalPosition() const
	{
		return goalPosList_;
	}

	const std::unordered_set<Vector2> &LevelMapChip::GetNeedlePosition() const
	{
		return needlePosList_;
	}

	std::unique_ptr<LevelMapChip::LevelMapChipHitBox> LevelMapChip::CreateHitBox(const std::function<bool(const MapChip &)> &checkFunc) const
	{
		auto result = std::make_unique<LevelMapChipHitBox>();

		// サイズの設定
		result->y_ = y_;
		result->x_ = x_;

		// メモリの確保
		result->hitBoxData_.resize(y_ * x_, false);
		// 変換して書き込む
		std::transform(mapChips_.begin(), mapChips_.end(), result->hitBoxData_.begin(), checkFunc);

		return std::move(result);
	}

	void LevelMapChipRenderer::Init(const LevelMapChip *levelMapChip) {
		pLevelMapChip_ = levelMapChip;
		CalcSpriteData();
	}

	void LevelMapChipRenderer::CalcSpriteData()
	{
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
				// マップチップの位置
				const auto mapChipPosition = CalcMapChipPosition(y, x);

				if (mapChipType == LevelMapChip::MapChip::kEmpty) {
					continue;
				}
#ifndef USE_IMGUI
				if (mapChipType == LevelMapChip::MapChip::kStart) {
					continue;
				}
#endif // USE_IMGUI

				// データの追加
				drawList[static_cast<size_t>(mapChipType)].emplace_back(mapChipPosition);
			}
		}


		size_t count = 0;
		// 描画テーブルのメモリ確保
		for (const auto &[index, positions] : drawList) {
			if (index == static_cast<size_t>(LevelMapChip::MapChip::kMesh)) {
				meshSpriteList_.resize(positions.size());
				continue;
			}
			count += positions.size();
		}
		spriteList_.resize(count);

		std::array<size_t, 2> listIndex = { 0 };
		// 描画テーブルへの追加
		for (const auto &[index, positions] : drawList) {
			const auto &mapChip = mapChips[index];
			// マップチップのテクスチャハンドル
			const TextureHandle textureHandle = mapChip.GetTextureHandle();
			// メモリの確保

			std::vector<std::unique_ptr<Sprite>> &spriteTarget = (index != static_cast<size_t>(LevelMapChip::MapChip::kMesh) ? spriteList_ : meshSpriteList_);
			size_t &i = (index != static_cast<size_t>(LevelMapChip::MapChip::kMesh) ? listIndex[0] : listIndex[1]);

			// 座標
			for (const auto &position : positions) {
				// もしスプライトが無かったら､そこにデータを与える
				if (auto &sprite = spriteTarget[i]; not sprite.get()) {
					sprite = Sprite::Generate();
				}

				Sprite *sprite = spriteTarget[i].get();

				sprite->SetTextureHaundle(textureHandle.index_);
				sprite->SetPosition(position);
				sprite->SetScale(mapChip.GetDrawScale());

				// スプライトの設定
				sprite->SetPivot(Vector2::one / 2);	// 中心に設定
				sprite->SetInvertY(true);			// UVのY軸反転

				if (index == static_cast<size_t>(LevelMapChip::MapChip::kFloor)) {
					sprite->SetTexDiff(Vector2::one * 128);
					sprite->SetTexOrigin(StageToDrawMap(position) * 128.f);
				}

				i += 1;
			}
		}
	}

	void LevelMapChipRenderer::Draw() {


		// 全てに対して描画を実行
		for (const auto &sprite : spriteList_) {
			sprite->Draw();
		}

		// 全てに対して描画を実行
		for (const auto &sprite : meshSpriteList_) {
			sprite->Draw();
		}

	}
	void LevelMapChipRenderer::DrawNet()
	{

		// 全てに対して描画を実行
		for (const auto &sprite : meshSpriteList_) {
			sprite->Draw();
		}
	}
	Vector2 LevelMapChipRenderer::CalcMapChipPosition(const uint32_t y, const uint32_t x) const
	{
		return Vector2(x * vMapChipScale_, y * vMapChipScale_);
	}

	Vector2 LevelMapChipRenderer::StageToDrawMap(Vector2 pos) {
		//	const int stage = *stagePointer;

		const auto [ySize, xSize] = pLevelMapChip_->GetSize();

		const auto &hitMap = *pLevelMapChip_;

		const auto BlockChip = LevelMapChip::MapChip::kFloor;

		const int32_t dx = static_cast<int32_t>(pos.x), dy = static_cast<int32_t>(pos.y);


		if ((*pLevelMapChip_)[dy][dx] == BlockChip) {
			std::bitset<10> mapChipConnect = { 0 };

			if (dy <= 0) {
				mapChipConnect[1] = true;
				mapChipConnect[2] = true;
				mapChipConnect[3] = true;
			}
			if (dy >= static_cast<int32_t>(ySize - 1)) {
				mapChipConnect[7] = true;
				mapChipConnect[8] = true;
				mapChipConnect[9] = true;

			}

			if (dx <= 0) {
				mapChipConnect[1] = true;
				mapChipConnect[4] = true;
				mapChipConnect[7] = true;
			}
			if (dx >= static_cast<int32_t>(xSize - 1)) {
				mapChipConnect[3] = true;
				mapChipConnect[6] = true;
				mapChipConnect[9] = true;
			}

			if (dy > 0) {
				if (dx > 0) {
					if (hitMap[dy - 1][dx - 1] == BlockChip) {
						mapChipConnect[1] = true;
					}
				}
				if (hitMap[dy - 1][dx] == BlockChip) {
					mapChipConnect[2] = true;
				}
				if (dx < static_cast<int32_t>(xSize - 1)) {
					if (hitMap[dy - 1][dx + 1] == BlockChip) {
						mapChipConnect[3] = true;
					}
				}
			}


			if (dx > 0) {
				if (hitMap[dy][dx - 1] == BlockChip) {
					mapChipConnect[4] = true;
				}
			}
			if (dx < static_cast<int32_t>(xSize - 1)) {
				if (hitMap[dy][dx + 1] == BlockChip) {
					mapChipConnect[6] = true;
				}
			}

			if (dy < static_cast<int32_t>(ySize - 1)) {
				if (dx > 0) {
					if (hitMap[dy + 1][dx - 1] == BlockChip) {
						mapChipConnect[7] = true;
					}
				}
				if (hitMap[dy + 1][dx] == BlockChip) {
					mapChipConnect[8] = true;
				}
				if (dx < static_cast<int32_t>(xSize - 1)) {
					if (hitMap[dy + 1][dx + 1] == BlockChip) {
						mapChipConnect[9] = true;
					}
				}
			}


			if ((!mapChipConnect[2] && !mapChipConnect[8]) && (!mapChipConnect[4] && !mapChipConnect[6])) {
				return { 3,3 };
			}


			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (!mapChipConnect[4] && !mapChipConnect[6])) {
				return { 3,0 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && !mapChipConnect[6])) {
				return { 3,1 };
			}
			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && !mapChipConnect[6])) {
				return { 3,2 };
			}

			else if ((!mapChipConnect[2] && !mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6])) {
				return { 0,3 };
			}
			else if ((!mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6])) {
				return { 1,3 };
			}
			else if ((!mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6])) {
				return { 2,3 };
			}


			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[3])) {
				return { 0,0 };
			}
			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[3])) {
				return { 1,0 };
			}
			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (mapChipConnect[1])) {
				return { 2,0 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[3] && mapChipConnect[9])) {
				return { 0,1 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[3] && mapChipConnect[7] && mapChipConnect[9])) {
				return { 1,1 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[7])) {
				return { 2,1 };
			}

			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[9])) {
				return { 0,2 };
			}
			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[7] && mapChipConnect[9])) {
				return { 1,2 };
			}
			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (mapChipConnect[7])) {
				return { 2,2 };
			}



			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[3])) {
				return { 4,0 };
			}

			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[3])) {
				return { 5,0 };
			}
			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[3])) {
				return { 6,0 };
			}
			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[3])) {
				return { 8,0 };
			}

			else if ((mapChipConnect[2] && !mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (!mapChipConnect[1])) {
				return { 7,0 };
			}



			else if ((mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[3] && mapChipConnect[9])) {
				return { 4,1 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[3] && mapChipConnect[7] && mapChipConnect[9])) {
				return { 5,1 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[3] && mapChipConnect[7] && mapChipConnect[9])) {
				return { 6,1 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[3] && mapChipConnect[7] && mapChipConnect[9])) {
				return { 8,1 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[7])) {
				return { 7,1 };
			}


			else if ((mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[3] && !mapChipConnect[9])) {
				return { 4,2 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[3] && mapChipConnect[7] && !mapChipConnect[9])) {
				return { 5,2 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[3] && !mapChipConnect[7] && mapChipConnect[9])) {
				return { 6,2 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && mapChipConnect[3] && !mapChipConnect[7] && !mapChipConnect[9])) {
				return { 8,2 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[7])) {
				return { 7,2 };
			}



			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[9])) {
				return { 4,3 };
			}

			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[7] && !mapChipConnect[9])) {
				return { 5,3 };
			}
			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[7] && mapChipConnect[9])) {
				return { 6,3 };
			}
			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[7] && !mapChipConnect[9])) {
				return { 8,3 };
			}

			else if ((!mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (!mapChipConnect[7])) {
				return { 7,3 };
			}



			else if ((mapChipConnect[2] && mapChipConnect[8]) && (!mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[3] && !mapChipConnect[9])) {
				return { 4,4 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[3] && mapChipConnect[7] && !mapChipConnect[9])) {
				return { 5,4 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[3] && !mapChipConnect[7] && mapChipConnect[9])) {
				return { 6,4 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && !mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[7])) {
				return { 7,4 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[3] && !mapChipConnect[7] && !mapChipConnect[9])) {
				return { 8,4 };
			}


			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[3] && mapChipConnect[7] && !mapChipConnect[9])) {
				return { 9,0 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[3] && !mapChipConnect[7] && mapChipConnect[9])) {
				return { 9,1 };
			}

			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && mapChipConnect[3] && !mapChipConnect[7] && !mapChipConnect[9])) {
				return { 9,2 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[3] && !mapChipConnect[7] && mapChipConnect[9])) {
				return { 9,3 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (mapChipConnect[1] && !mapChipConnect[3] && !mapChipConnect[7] && !mapChipConnect[9])) {
				return { 10,2 };
			}
			else if ((mapChipConnect[2] && mapChipConnect[8]) && (mapChipConnect[4] && mapChipConnect[6]) && (!mapChipConnect[1] && !mapChipConnect[3] && mapChipConnect[7] && !mapChipConnect[9])) {
				return { 10,3 };
			}

		}

		return {};
	}
}
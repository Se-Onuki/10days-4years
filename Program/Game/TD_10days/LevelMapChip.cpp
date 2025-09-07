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
				case MapChipType::kStart: {
					startPos_ = Vector2{ static_cast<float>(xi), static_cast<float>(yi) };
					break;
				}
				case MapChipType::kGoal: {
					goalPosList_.insert(Vector2{ static_cast<float>(xi), static_cast<float>(yi) });
					break;
				}
				case MapChipType::kNeedle: {
					needlePosList_.insert(Vector2{ static_cast<float>(xi), static_cast<float>(yi) });
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

	const LevelMapChip::LevelMapChipHitBox *LevelMapChip::CreateHitBox()
	{
		// 当たり判定がないなら作る
		if (not hitBox_) {
			hitBox_ = std::make_unique<LevelMapChipHitBox>();
		}
		// サイズの設定
		hitBox_->y_ = y_;
		hitBox_->x_ = x_;
		// メモリの確保
		hitBox_->hitBoxData_.resize(y_ * x_, false);
		// 変換して書き込む
		std::transform(mapChips_.begin(), mapChips_.end(), hitBox_->hitBoxData_.begin(), [this](const MapChip &chip) {
			// 当たり判定のフラグを持っているならtrue
			return mapChipData_[static_cast<uint32_t>(chip)].IsHasCollision();
			});
		return hitBox_.get();
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
				// データの追加
				drawList[static_cast<size_t>(mapChipType)].emplace_back(mapChipPosition);
			}
		}


		size_t count = 0;
		// 描画テーブルのメモリ確保
		for (const auto &[index, positions] : drawList) {
			count += positions.size();
		}
		spriteList_.resize(count);

		size_t i = 0;
		// 描画テーブルへの追加
		for (const auto &[index, positions] : drawList) {
			// マップチップのテクスチャハンドル
			const TextureHandle textureHandle = mapChips[index].GetTextureHandle();
			// メモリの確保

			// 座標
			for (const auto &position : positions) {
				// もしスプライトが無かったら､そこにデータを与える
				if (auto &sprite = spriteList_[i]; not sprite.get()) {
					sprite = Sprite::Generate();
				}

				// スプライトの生成
				Sprite *sprite = spriteList_[i].get();

				sprite->SetTextureHaundle(textureHandle.index_);
				sprite->SetPosition(position);

				// スプライトの設定
				sprite->SetPivot(Vector2::one / 2);	// 中心に設定
				sprite->SetInvertY(true);			// UVのY軸反転
				i += 1;
			}
		}
	}

	void LevelMapChipRenderer::Draw() {


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
#include "Water.h"

namespace TD_10days {

	void Water::Init(const Vector2 &position, const Vector2 &size, const uint32_t color)
	{
		position_ = position;
		size_ = size;
		color_ = color;

		isActive_ = false;

		DeleteWater();

		chainWater_ = std::make_unique<ChainWater>();
		chainWater_->Init(size, color);
		chainWater_->CreateChain(position);
		waterParticleManager_->SpawnParticle(position);
	}

	void Water::Update(const float deltaTime) {
		if (not isActive_) {
			return;
		}
		if (lifeTime_ <= 0.f) {
			isActive_ = false;
			DeleteWater();
			if (chainWater_) {
				chainWater_->CalcUpdate();
			}
		}
		else {
			lifeTime_ -= deltaTime;
		}
	}

	void Water::Draw() const {
		if (not chainWater_) { return; }
		chainWater_->Draw();
	}

	void Water::DeleteWater() {
		if (not chainWater_) { return; }

		waterParticleManager_->Collapse();
		chainWater_->Clear();
	}

	void Water::PlacementWater(const Vector2 &direction)
	{
		if (not chainWater_) {
			chainWater_ = std::make_unique<ChainWater>();
			chainWater_->Init(size_, color_);
		}

		chainWater_->MoveDirection(direction);
		waterParticleManager_->MoveDirection(direction);
		chainWater_->CreateChain(position_);
		waterParticleManager_->SpawnParticle(position_);
	}

	bool Water::IsPlaceAble(const TD_10days::LevelMapChip::LevelMapChipHitBox *hitBox, const Vector2 &direction) const
	{
		const auto waterPos = GetWaterPosition();

		for (const Vector2 &pos : waterPos) {
			// 次の移動先の座標
			const Vector2 nextPos = pos + direction;
			// 既存の設置座標と合致した場合､置けない
			if (position_ == nextPos) {
				return false;
			}

			// もし当たり判定と一致した場合は移動できない｡
			if (nextPos.x < 0 or nextPos.y < 0 or nextPos.x >= hitBox->GetX() or nextPos.y >= hitBox->GetY() or hitBox->at(static_cast<size_t>(nextPos.y), static_cast<size_t>(nextPos.x))) {
				return false;
			}

		}

		return true;
	}

	void Water::Activate(float maxTime)
	{
		isActive_ = true;
		lifeTime_ = maxTime;
	}


	const std::unordered_set<Vector2> Water::GetWaterPosition() const
	{
		if (not chainWater_) { return {}; }

		return chainWater_->GetPositionList();
	}

	std::optional<float> Water::GetWaterTime() const
	{
		std::optional<float> result;
		if (isActive_) { result = lifeTime_; }
		return result;
	}

	void Water::ChainWater::Init(const Vector2 &size, const uint32_t color)
	{
		size_ = size;
		color_ = color;
	}


	void Water::ChainWater::CalcUpdate()
	{

		//for (auto &water : chainWaterList_) {
		//	water->sprite_->CalcBuffer();
		//}
	}

	void Water::ChainWater::MoveDirection(const Vector2 &direction)
	{
		// 自分以降の水をすべて動かす
		for (auto &water : chainWaterList_) {
			water->SetPosition(water->position_ + direction);
		}
	}

	std::unordered_set<Vector2> Water::ChainWater::GetPositionList() const
	{
		std::unordered_set<Vector2> result;
		// 個数を合わせる
		result.reserve(chainWaterList_.size());
		// 水の座標を抽出
		std::transform(chainWaterList_.begin(), chainWaterList_.end(), std::inserter(result, result.end()), [](const std::unique_ptr<ChainWaterData> &water) { return water->position_; });
		return result;
	}

	void Water::ChainWater::Clear()
	{
		chainWaterList_.clear();
	}

	void Water::ChainWater::Draw() const {
		/*for (const auto &water : chainWaterList_) {
			water->sprite_->Draw();
		}*/
	}

	std::unique_ptr<Water::ChainWater::ChainWaterData> Water::ChainWater::ChainWaterData::Generate(uint32_t) {
		auto newChain = std::make_unique<ChainWaterData>();

		//newChain->sprite_ = Sprite::Generate(TextureManager::LoadDefaultTexture());
		//newChain->sprite_->SetColor(color);
		//newChain->sprite_->SetPivot(Vector2::one * 0.5f);
		//newChain->sprite_->SetInvertY(true);

		//newChain->sprite_->CalcBuffer();

		newChain->position_ = Vector2::zero;

		return std::move(newChain);
	}

	Water::ChainWater::ChainWaterData *Water::ChainWater::CreateChain(const Vector2 &position) {
		auto newChain = ChainWaterData::Generate(color_);
		// 位置の設定
		newChain->SetPosition(position);

		return chainWaterList_.emplace_back(std::move(newChain)).get();
	}

	void Water::ChainWater::ChainWaterData::SetPosition(const Vector2 &position) {
		position_ = position;
		/*if (sprite_) {
			sprite_->SetPosition(position_);
			sprite_->CalcBuffer();
		}*/
	}

}

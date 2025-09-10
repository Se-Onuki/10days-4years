#include "WaterEffect.h"

#undef min
#undef max
#include <algorithm>
#include <Utils/SoLib/SoLib_Easing.h>

namespace TD_10days
{
	void WaterParticle::Init(const Vector2 &position, const Rect &bounds)
	{
		sprite_ = Sprite::Generate(TextureManager::Load("output1.png"));
		sprite_->SetScale(Vector2{ initialScale_, initialScale_ });
		sprite_->SetPivot(Vector2{ 0.5f, 0.5f });
		sprite_->SetColor(/*0x0080FFFF*/0xFFFFFF99);
		position_ = position;
		startPosition_ = position;
		sprite_->SetPosition(position_);
		bounds_ = bounds;
		dropTime_ = 0.0f;
	}
	void WaterParticle::Update(float deltaTime)
	{
		if (/*onGround_*/not inBoundary_) {
			// 寿命を減らす
			lifeTime_ -= deltaTime;

			// 残り寿命の割合を計算 (0.0f ～ 1.0f)
			const float invT = std::max(lifeTime_ / survivalTime_, 0.0f);

			// 線形補間で当たり判定を小さく
			sprite_->SetColor({ 1.f,1.f,1.f,(invT) * (99.f / 0xFF) });
			radius_ = defaultRadius_ * invT;

			// 寿命が尽きたら非アクティブ化
			if (lifeTime_ <= 0.0f) {
				isActive_ = false;
			}
			velocity_.y += gravity_;
		}
		else {
			velocity_.y += 0.001f;

			if (dropTime_ > 3.0f) {
				dropTime_ = 0.0f;
			}
			dropTime_ += deltaTime;
			
		}
		position_ += velocity_;
		sprite_->SetPosition(position_);
	}
	void WaterParticle::Draw()
	{
		sprite_->Draw();

		for (const auto &sprite : boundsSprites_) {
			sprite->Draw();
		}
	}

	void WaterParticleManager::Init()
	{
		particles_.clear();
		drops_.clear();
	}

	void WaterParticleManager::Update(const LevelMapChip::LevelMapChipHitBox *hitBox, float chipSize, float deltaTime)
	{

		time_ += deltaTime;

		std::vector<Rect> boundList;

		for (const auto &particle : particles_) {
			particle->Update(deltaTime);

			Rect bounds = particle->GetBounds();
			if (particle->GetDropTime() > 3.0f and particle->GetInBoundary()) {
				if (boundList.size() == 0) {
					std::unique_ptr<WaterParticle> drop = std::make_unique<WaterParticle>();
					drop->Init(Vector2{ SoLib::Random::GetRandom(bounds.left, bounds.right), SoLib::Random::GetRandom(bounds.bottom, bounds.top) }, Rect{ 0.0f, 0.0f, 0.0f, 0.0f });
					drop->SetInBoundary(false);
					drop->SetSize(Vector2{ 0.2f, 0.2f });
					drops_.emplace_back(std::move(drop));

					boundList.push_back(bounds);
				}
				else {

					for (const auto& bound : boundList) {
						if (bounds.left != bound.left or bounds.right != bound.right or bounds.top != bound.top or bounds.bottom != bound.bottom) {
							std::unique_ptr<WaterParticle> drop = std::make_unique<WaterParticle>();
							drop->Init(Vector2{ SoLib::Random::GetRandom(bounds.left, bounds.right), SoLib::Random::GetRandom(bounds.bottom, bounds.top) }, Rect{ 0.0f, 0.0f, 0.0f, 0.0f });
							drop->SetInBoundary(false);
							drop->SetSize(Vector2{ 0.2f, 0.2f });
							drops_.emplace_back(std::move(drop));

							boundList.push_back(bounds);
						}

					}
				}
				
			}
		
		}

		particles_.remove_if([](const std::unique_ptr<WaterParticle> &particle) {
			return not particle->GetActive();
			});

		for (const auto& particle : drops_) {
			particle->Update(deltaTime);
		}

		drops_.remove_if([](const std::unique_ptr<WaterParticle>& particle) {
			return not particle->GetActive();
			});

		Collider(hitBox, chipSize);
	}

	void WaterParticleManager::Draw()
	{
		for (const auto &particle : particles_) {
			particle->Draw();
		}

		for (const auto& particle : drops_) {
			particle->Draw();
		}
	}

	void WaterParticleManager::SpawnParticle(Vector2 position)
	{
		const float radius = 0.5f;

		const size_t kCount = 6;

		for (int y = 0; y < kCount; y++) {
			for (int x = 0; x < kCount; x++) {
				std::unique_ptr<WaterParticle> particle = std::make_unique<WaterParticle>();
				particle->Init(Vector2{ position.x - 0.5f + x * (1.f / kCount), position.y - 0.5f + y * (1.f / kCount) }, Rect{ position.x - radius, position.x + radius, position.y - radius, position.y + radius });
				particles_.emplace_back(std::move(particle));
			}
		}
	}

	void WaterParticleManager::MoveDirection(const Vector2 &direction)
	{
		for (const auto &particle : particles_) {
			if (particle->GetIsMovable()) {
				particle->SetPosition(particle->GetPosition() + direction *0.5f);
				particle->SetVelocity(direction * 0.125f);
				particle->SetBounds(Rect{ particle->GetBounds().left + direction.x, particle->GetBounds().right + direction.x, particle->GetBounds().top + direction.y, particle->GetBounds().bottom + direction.y });
			}
		}
	}

	void WaterParticleManager::Collapse()
	{
		for (const auto &particle : particles_) {
			particle->SetInBoundary(false);
		}
	}

	void WaterParticleManager::Fixed()
	{
		for (const auto &partilce : particles_) {
			partilce->SetIsMovable(false);
		}
	}

	void WaterParticleManager::Collider(const LevelMapChip::LevelMapChipHitBox *hitBox, float chipSize)
	{
		// --- 粒子同士の衝突 ---
		for (auto it1 = particles_.begin(); it1 != particles_.end(); ++it1) {
			auto it2 = it1;
			++it2; // it1の次からスタート
			for (; it2 != particles_.end(); ++it2) {
				CircleCircle(it1->get(), it2->get(), 0.2f);
			}
		}

		const float kBoxRadius = 0.6f;

		for (auto &p : particles_) {
			if (p->GetInBoundary()) { /// --- 境界との衝突 ---
				CircleBounds(p.get(), p->GetBounds(), 0.8f);
			}
			else { // --- マップチップとの衝突 ---
				if (hitBox) {
					Vector2 pos = p->GetPosition();
					int chipX = static_cast<int>(std::floor(pos.x / chipSize));
					int chipY = static_cast<int>(std::floor(pos.y / chipSize));

					// 近傍チップをチェック
					for (int dy = -1; dy <= 1; ++dy) {
						for (int dx = -1; dx <= 1; ++dx) {
							int nx = chipX + dx;
							int ny = chipY + dy;

							if (hitBox->at(ny, nx)) {
								// チップをRectに変換
								Rect rect;
								rect.left = nx - kBoxRadius;
								rect.right = nx + kBoxRadius;
								rect.top = ny - kBoxRadius;
								rect.bottom = ny + kBoxRadius;

								CircleAABB(p.get(), rect, 0.5f);
							}
						}
					}
				}
			}
		}

		// --- 粒子同士の衝突 ---
		for (auto it1 = particles_.begin(); it1 != particles_.end(); ++it1) {
			for (auto it2 = drops_.begin(); it2 != drops_.end(); ++it2) {
				CircleCircle(it1->get(), it2->get(), 0.2f);
			}
		}

		for (auto& p : drops_) {
			if (p->GetInBoundary()) { /// --- 境界との衝突 ---
				CircleBounds(p.get(), p->GetBounds(), 0.8f);
			}
			else { // --- マップチップとの衝突 ---
				if (hitBox) {
					Vector2 pos = p->GetPosition();
					int chipX = static_cast<int>(std::floor(pos.x / chipSize));
					int chipY = static_cast<int>(std::floor(pos.y / chipSize));

					// 近傍チップをチェック
					for (int dy = -1; dy <= 1; ++dy) {
						for (int dx = -1; dx <= 1; ++dx) {
							int nx = chipX + dx;
							int ny = chipY + dy;

							if (hitBox->at(ny, nx)) {
								// チップをRectに変換
								Rect rect;
								rect.left = nx - kBoxRadius;
								rect.right = nx + kBoxRadius;
								rect.top = ny - kBoxRadius;
								rect.bottom = ny + kBoxRadius;

								CircleAABB(p.get(), rect, 0.5f);
							}
						}
					}
				}
			}



		}

	}

	void WaterParticleManager::CircleCircle(WaterParticle *p1, WaterParticle *p2, float restitution)
	{
		if (not p1->GetActive() || not p2->GetActive()) {
			return;
		}

		Vector2 pos1 = p1->GetPosition();
		Vector2 pos2 = p2->GetPosition();
		Vector2 vel1 = p1->GetVelocity();
		Vector2 vel2 = p2->GetVelocity();

		Vector2 diff = { pos2.x - pos1.x, pos2.y - pos1.y };
		float dist2 = diff.x * diff.x + diff.y * diff.y;
		float minDist = p1->GetRadius() + p2->GetRadius();

		// 接触判定
		if (dist2 < minDist * minDist && dist2 > 0.0001f) {
			float dist = std::sqrt(dist2);
			Vector2 normal = { diff.x / dist, diff.y / dist };

			// --- 位置修正 ---
			float overlap = 0.0f;
			/*if (activeFloor) {
				overlap = (minDist - dist) * 0.1f;
			}
			else {
				overlap = (minDist - dist) * 0.4f;
			}*/
			overlap = (minDist - dist) * 0.4f;

			pos1.x -= overlap * normal.x;
			pos1.y -= overlap * normal.y;
			pos2.x += overlap * normal.x;
			pos2.y += overlap * normal.y;

			// --- 速度修正 ---
			Vector2 relVel = { vel2.x - vel1.x, vel2.y - vel1.y };
			float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

			if (velAlongNormal < 0) { // 近づいている場合のみ反射
				float impulse = -(1.0f + restitution) * velAlongNormal * 0.5f;
				Vector2 impulseVec = { impulse * normal.x, impulse * normal.y };

				vel1.x -= impulseVec.x;
				vel1.y -= impulseVec.y;
				vel2.x += impulseVec.x;
				vel2.y += impulseVec.y;
			}

			// 更新
			p1->SetPosition(pos1);
			p2->SetPosition(pos2);
			p1->SetVelocity(vel1);
			p2->SetVelocity(vel2);

			return;
		}

		return;
	}
	void WaterParticleManager::CircleAABB(WaterParticle *p, const Rect &rect, float restitution)
	{
		if (not p->GetActive()) {
			return;
		}

		Vector2 pos = p->GetPosition();
		Vector2 vel = p->GetVelocity();
		float radius = p->GetRadius();

		// 最近点を求める
		float nearestX = std::max(rect.left, std::min(pos.x, rect.right));
		float nearestY = std::max(rect.top, std::min(pos.y, rect.bottom));

		// 円の中心とのベクトル
		float dx = pos.x - nearestX;
		float dy = pos.y - nearestY;
		float distSq = dx * dx + dy * dy;

		// 衝突していない
		if (distSq > radius * radius) { return; }

		p->SetOnGround(true);

		float dist = std::sqrt(distSq);

		// 中心が矩形の中にある場合の特殊処理
		if (dist == 0.0f) {
			// x方向かy方向どちらに押し出すかを決める
			float moveLeft = std::abs(pos.x - rect.left);
			float moveRight = std::abs(rect.right - pos.x);
			float moveTop = std::abs(pos.y - rect.top);
			float moveBottom = std::abs(rect.bottom - pos.y);

			float minMove = std::min({ moveLeft, moveRight, moveTop, moveBottom });

			if (minMove == moveLeft) {
				p->SetPosition(Vector2{ rect.left - radius , pos.y });
			}
			else if (minMove == moveRight) {
				p->SetPosition(Vector2{ rect.right + radius , pos.y });
			}
			else if (minMove == moveTop) {
				p->SetPosition(Vector2{ pos.x ,rect.top - radius });
			}
			else if (minMove == moveBottom) {
				p->SetPosition(Vector2{ pos.x, rect.bottom + radius });
			}
		}
		else {
			// 押し出しベクトル = 円半径 - 距離
			float overlap = radius - dist;
			float nx = dx / dist;
			float ny = dy / dist;

			// 円の中心を移動させる
			p->SetPosition(pos + Vector2{ nx * overlap , ny * overlap });
		}

		p->SetVelocity(p->GetVelocity().Reflect(Vector2{ dx, dy }.Normalize()) * restitution);
		return;
	}
	void WaterParticleManager::CircleBounds(WaterParticle *p, const Rect &rect, float restitution)
	{
		if (not p->GetActive()) {
			return;
		}

		Vector2 pos = p->GetPosition();
		Vector2 vel = p->GetVelocity();
		float r = p->GetRadius();

		// --- 下 ---
		if (pos.y + r > rect.bottom) {
			pos.y = rect.bottom - r;
			vel.y *= -restitution;
		}
		// --- 上 ---
		if (pos.y - r < rect.top) {
			pos.y = rect.top + r;
			vel.y *= -restitution;
		}
		// --- 左 ---
		if (pos.x - r < rect.left) {
			pos.x = rect.left + r;
			vel.x *= -restitution;
		}
		// --- 右 ---
		if (pos.x + r > rect.right) {
			pos.x = rect.right - r;
			vel.x *= -restitution;
		}

		p->SetPosition(pos);
		p->SetVelocity(vel);

		return;
	}
}
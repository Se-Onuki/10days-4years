#include "WaterEffect.h"

#undef min
#undef max
#include <algorithm>

namespace TD_10days
{
	WaterEffect::WaterEffect()
	{
		input_ = SolEngine::Input::GetInstance();
	}
	void WaterEffect::Init()
	{
		SpawnWater(Vector2{ 640.0f, 360.0f });
		SpawnWater(Vector2{ 100.0f, 360.0f });
		SpawnWater(Vector2{ 1200.0f, 360.0f });
		SpawnSplash(Vector2{ 640.0f, 300.0f }, Vector2{ 0.0f, 1.0f });

	}

	void WaterEffect::Update(/*float deltaTime*/)
	{
		if (input_->GetDirectInput()->IsTrigger(DIK_R)) {
			particles_.clear();
			SpawnWater(Vector2{ 640.0f, 360.0f });
			SpawnWater(Vector2{ 100.0f, 360.0f });
			SpawnWater(Vector2{ 1200.0f, 360.0f });
			activeFloor_ = true;

			splashParticles_.clear();
			SpawnSplash(Vector2{ 640.0f, 300.0f }, Vector2{ 0.0f, 0.0f });
		}

		for (const auto& particle : particles_) {

			particle->Update(0.5f);
		}

		for (const auto& particle : splashParticles_) {
			particle->Update(0.1f);
		}



		Collisiton();
	}

	void WaterEffect::Draw()
	{
		for (const auto& particle : particles_) {
			particle->Draw();
		}

		for (const auto& particle : splashParticles_) {
			particle->Draw();
		}
	}
	void WaterEffect::Collider()
	{
	}
	bool WaterEffect::CircleCircle(WaterParticle* p1, WaterParticle* p2, float restitution, bool activeFloor)
	{
		if (!p1->GetActive() || !p2->GetActive()) {
			return false;
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
			if (activeFloor) {
				overlap = (minDist - dist) * 0.1f;
			}
			else {
				overlap = (minDist - dist) * 0.4f;
			}

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

			return true;
		}

		return false;
	}
	bool WaterEffect::CircleAABB(WaterParticle* p, const Rect& rect, float restitution)
	{
		if (!p->GetActive()) {
			return false;
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
		if (distSq > radius * radius) { return false; }

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
		return true;
	}
	void WaterEffect::CircleBounds(WaterParticle* p, const Rect& rect, float restitution)
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
	void WaterEffect::Collisiton()
	{
		// --- 粒子同士の衝突 ---
		for (size_t i = 0; i < particles_.size(); i++) {
			for (size_t j = i + 1; j < particles_.size(); j++) {
				if (CircleCircle(particles_[i].get(), particles_[j].get(), 0.2f, activeFloor_)) {

				}
			}
		}

		// --- 境界との衝突 ---
		if (input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
			activeFloor_ = false;
		}
		Rect bounds = { 600.0f, 700.0f, 0.0f, 370.0f };
		if( not activeFloor_) {
			bounds = { 0.0f, 1280.0f, 0.0f, 670.0f };
		}

		for (auto& p : particles_) {

			if (not activeFloor_) {
				CircleBounds(p.get(), bounds, 0.8f);
			}
			else {
				CircleBounds(p.get(), p->GetBounds()/*bounds*/, 0.8f);
			}
			

			// --- 壁との衝突 ---
			Rect rect = { 520.0f, 760.0f, 600.0f, 650.0f };
			if (CircleAABB(p.get(), rect, 0.8f)) {

			}
			rect = { 120.0f, 360.0f, 600.0f, 650.0f };
			if (CircleAABB(p.get(), rect, 0.8f)) {

			}
			rect = { 780.0f, 1200.0f, 600.0f, 650.0f };
			if(CircleAABB(p.get(), rect, 0.8f)) {
			}

		}
	}
	void WaterEffect::SpawnWater(Vector2 position)
	{
		for (int i = 0; i < 64; i++) {

			std::unique_ptr<WaterParticle> particle = std::make_unique<WaterParticle>();
			particle->Init(Vector2{ position.x + 1.0f + i * 2.0f, position.y + 1.0f - i }, Rect{position.x - 100.0f / 2.0f, position.x + 100.0f / 2.0f, position.y - 100.0f / 2.0f, position.y + 100.0f / 2.0f });
			particles_.emplace_back(std::move(particle));
		}

	}
	void WaterEffect::SpawnSplash(Vector2 position, Vector2 velocity)
	{
		for (int i = 0; i < 10; i++) {
			std::unique_ptr<SplashParticle> particle = std::make_unique<SplashParticle>();
			particle->Init(position, velocity);
			splashParticles_.push_back(std::move(particle));
		}
	}
	void WaterParticle::Init(const Vector2& position, const Rect& bounds)
	{
		sprite_ = Sprite::Generate(TextureManager::Load("output1.png"));
		sprite_->SetScale(Vector2{ 16.0f, 16.0f }*6.f);
		sprite_->SetPivot(Vector2{ 0.5f, 0.5f });
		sprite_->SetColor(0xFFFFFFFF);
		position_ = position;
		startPosition_ = position;
		sprite_->SetPosition(position_);
		bounds_ = bounds;
	}
	void WaterParticle::Update(float gravity)
	{
		if (isActive_) {
			velocity_.y += gravity;
		}
		position_ += velocity_;
		sprite_->SetPosition(position_);

		//Vector4 color = std::clamp((720.0f - position_.y) / (720.0f - startPosition_.y), 0.0f, 1.0f));
		/*sprite_->SetColor(Vector4{0.0f, 0.0f, 1.0f, std::clamp((720.0f - position_.y) / (720.0f - startPosition_.y), 0.0f, 1.0f)});

		if (std::clamp((720.0f - position_.y) / (720.0f - startPosition_.y), 0.0f, 1.0f) < 0.1f) {
			isActive_ = false;
		}*/

	}
	void WaterParticle::Draw()
	{
		sprite_->Draw();
	}

}
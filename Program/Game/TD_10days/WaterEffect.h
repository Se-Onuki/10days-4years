#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>
#include <algorithm>

#include "../Engine/DirectBase/Input/Input.h"
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャの情報
#include "../../Engine/Utils/Math/Random.h"
#include "../../Engine/DirectBase/File/VariantItem.h"		// 外部編集可能な変数の定義
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../Game/TD_10days/LevelMapChip.h"

namespace TD_10days {
	
	struct Rect {
		float left;
		float right;
		float top;
		float bottom;
	};

	class WaterParticle {
	public:
		WaterParticle() = default;
		~WaterParticle() = default;
		void Init(const Vector2& position, const Rect& bounds);
		void Update(float deltaTime);
		void Draw();

		void SpawnSplash(Vector2 position, Vector2 velocity);

		bool GetActive() const { return isActive_; }
		float GetRadius() const { return radius_; }
		Vector2 GetPosition() const { return position_; }
		Vector2 GetVelocity() const { return velocity_; }
		Rect GetBounds() const { return bounds_; }
		bool GetInBoundary() const { return inBoundary_; }

		void SetActive(bool isActive) { isActive_ = isActive; }
		void SetSize(const Vector2& size) { sprite_->SetScale(size); }
		void SetPosition(const Vector2& position) { position_ = position; }
		void SetVelocity(const Vector2& velocity) { velocity_ = velocity; }
		void SetColor(const Vector4& color) { sprite_->SetColor(color); }
		void SetBounds(const Rect& bounds) { bounds_ = bounds; }
		void SetInBoundary(bool inBoundary) { inBoundary_ = inBoundary; }
		void SetOnGround(bool onGround) { onGround_ = onGround; }


	private:
		std::unique_ptr<Sprite> sprite_;
		std::vector<std::unique_ptr<Sprite>> boundsSprites_;

		bool isActive_ = true;
		bool inBoundary_ = true;
		bool onGround_ = false;

		float gravity_ = -0.05f;
		float radius_ = 0.1f;
		static inline constexpr float defaultRadius_ = 0.1f;
		static inline constexpr float initialScale_ = 0.5f;
		static inline constexpr float survivalTime_ = 3.0f;
		float lifeTime_ = 3.0f;

		Vector2 basePosition_{};
		Vector2 velocity_{};
		Vector2 position_{};
		Vector2 startPosition_{};

		Rect bounds_{};

	};

	class WaterParticleManager {
	public:
		WaterParticleManager() = default;
		~WaterParticleManager() = default;
		void Init();
		void Update(const LevelMapChip::LevelMapChipHitBox* hitBox, float chipSize, float deltaTime);
		void Draw();
		void SpawnParticle(Vector2 position);
		void MoveDirection(const Vector2& direction);
		void Collapse();

	private:
		void Collider(const LevelMapChip::LevelMapChipHitBox* hitBox, float chipSize);
		void CircleCircle(WaterParticle* p1, WaterParticle* p2, float restitution/*, bool activeFloor*/);
		void CircleAABB(WaterParticle* p, const Rect& rect, float restitution);
		void CircleBounds(WaterParticle* p, const Rect& rect, float restitution);

	private:
		float side_ = 0.5f;
		std::list<std::unique_ptr<WaterParticle>> particles_;
	};

}



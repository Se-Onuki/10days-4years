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
#include "SplashParticle.h"

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
		void Update(float gravity);
		void Draw();

		void SpawnSplash(Vector2 position, Vector2 velocity);

		bool GetActive() const { return isActive_; }
		float GetRadius() const { return radius_; }
		Vector2 GetPosition() const { return position_; }
		Vector2 GetVelocity() const { return velocity_; }
		Rect GetBounds() const { return bounds_; }

		void SetActive(bool isActive) { isActive_ = isActive; }
		void SetSize(const Vector2& size) { sprite_->SetScale(size); }
		void SetPosition(const Vector2& position) { position_ = position; }
		void SetVelocity(const Vector2& velocity) { velocity_ = velocity; }
		void SetColor(const Vector4& color) { sprite_->SetColor(color); }
		void SetBounds(const Rect& bounds) { bounds_ = bounds; }


	private:
		std::unique_ptr<Sprite> sprite_;
		Vector2 velocity_{};
		Vector2 position_{};
		Vector2 startPosition_{};
		float radius_ = 4.0f;
		bool isActive_ = true;
		Rect bounds_{};
	};

	class WaterEffect
	{
	public:
		WaterEffect();
		~WaterEffect() = default;
		void Init();
		void Update(/*float deltaTime*/);
		void Draw();
	private:
		void Collider();
		static bool CircleCircle(WaterParticle* p1, WaterParticle* p2, float restitution, bool activeFloor);
		static bool CircleAABB(WaterParticle* p, const Rect& rect, float restitution);
		void CircleBounds(WaterParticle* p, const Rect& rect, float restitution);
		void Collisiton();
		void SpawnWater(Vector2 position);
		void SpawnSplash(Vector2 position, Vector2 velocity);
	private:
		/// @brief 入力のインスタンス
		SolEngine::Input* input_ = nullptr;
		bool activeFloor_ = true;
		std::vector<std::unique_ptr<WaterParticle>> particles_;
		std::vector<std::unique_ptr<SplashParticle>> splashParticles_;
	};
}



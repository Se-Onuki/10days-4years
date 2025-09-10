#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>
#include <algorithm>

#include "SplashParticle.h"
#include "../Engine/DirectBase/Render/CameraManager.h"
#include "BackgroundParticle.h"
#include "StarParticle.h"

namespace TD_10days {
	class ParticleManager {
		struct Size {
			float min;
			float max;
		};
	public:
		ParticleManager() = default;
		~ParticleManager() = default;
		void Init();
		void Update(float deltaTime);
		void Draw();
		void DrawBack();

		void SpawnSplash(Vector2 position, Vector2 velocity, bool isEnter);
		void SpawnBackground(SolEngine::Camera2D* camera);
		void SpawnStar(int count);

	private:
		std::list<std::unique_ptr<SplashParticle>> splashParticles_;
		std::vector<std::unique_ptr<BackgroundParticle>> backgroundParticles_;
		std::vector<std::unique_ptr<StarParticle>> starParticles_;

	};
}
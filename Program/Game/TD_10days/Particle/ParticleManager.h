#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>
#include <algorithm>

#include "SplashParticle.h"

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

		void SpawnSplash(Vector2 position, Vector2 velocity, bool isEnter);

	private:
		std::list<std::unique_ptr<SplashParticle>> splashParticles_;

	};
}
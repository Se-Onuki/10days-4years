#include "ParticleManager.h"

void TD_10days::ParticleManager::Init() {
	splashParticles_.clear();
}

void TD_10days::ParticleManager::Update(float deltaTime) {
	for (const auto& particle : splashParticles_) {
		particle->Update(deltaTime, -0.01f);
	}
}

void TD_10days::ParticleManager::Draw() {
	for (const auto& particle : splashParticles_) {
		particle->Draw();
	}
}

void TD_10days::ParticleManager::SpawnSplash(Vector2 position, Vector2 velocity)
{
	for (int i = 0; i < 10; i++) {
		std::unique_ptr<SplashParticle> particle = std::make_unique<SplashParticle>();
		particle->Init(position, velocity);
		splashParticles_.push_back(std::move(particle));
	}
}
#include "ParticleManager.h"

void TD_10days::ParticleManager::Init() {
	splashParticles_.clear();
	backgroundParticles_.clear();
	starParticles_.clear();	
}

void TD_10days::ParticleManager::Update(float deltaTime) {
	for (const auto& particle : splashParticles_) {
		particle->Update(deltaTime, -0.01f);
	}

	// 背景水滴更新
	for (auto& drop : backgroundParticles_) {
		drop->Update(deltaTime);
	}

	for (auto& drop : starParticles_) {
		drop->Update(deltaTime);
	}
}

void TD_10days::ParticleManager::Draw() {
	for (const auto& particle : splashParticles_) {
		particle->Draw();
	}

	for (const auto& particle : starParticles_) {
		particle->Draw();
	}
}

void TD_10days::ParticleManager::DrawBack()
{
	for (auto& drop : backgroundParticles_) {
		drop->Draw();
	}
}

void TD_10days::ParticleManager::SpawnSplash(Vector2 position, Vector2 velocity, bool isEnter)
{
	if (velocity.Length() == 0.0f) {
		return;
	}

	Vector2 playerVector = velocity;
	if (isEnter) {
		playerVector *= -1.0f;
	}

	Vector2 dir = velocity;
	dir.Normalize();
	Vector2 spwanPosition = position;
	if (fabs(dir.x) > fabs(dir.y)) {
		if (isEnter) {
			spwanPosition.x += 0.5f * (dir.x / fabs(dir.x));
		}
		else {
			spwanPosition.x -= 0.5f * (dir.x / fabs(dir.x));
		}
		
	}
	else {
		if (isEnter) {
			spwanPosition.y += 0.5f * (dir.y / fabs(dir.y));
		}
		else {
			spwanPosition.y -= 0.5f * (dir.y / fabs(dir.y));
		}
	}

	for (int i = 0; i < 10; i++) {
		std::unique_ptr<SplashParticle> particle = std::make_unique<SplashParticle>();
		particle->Init(spwanPosition, playerVector);
		splashParticles_.push_back(std::move(particle));
	}
	
}

void TD_10days::ParticleManager::SpawnBackground(SolEngine::Camera2D* camera)
{
	for (int i = 0; i < 25; i++) {
		auto drop = std::make_unique<BackgroundParticle>();
		drop->Init(*camera); // 画面サイズを渡す
		drop->SetCamera(camera);
		backgroundParticles_.push_back(std::move(drop));
	}
}

void TD_10days::ParticleManager::SpawnStar(int count)
{
	for (int i = 0; i < count; i++) {
		auto drop = std::make_unique<StarParticle>();
		drop->Init(); 
		starParticles_.push_back(std::move(drop));
	}
}

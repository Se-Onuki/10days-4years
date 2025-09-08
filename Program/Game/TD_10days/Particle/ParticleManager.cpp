#include "ParticleManager.h"

void TD_10days::ParticleManager::Init() {
	splashParticles_.clear();
}

void TD_10days::ParticleManager::Update(float deltaTime) {
	const auto input = SolEngine::Input::GetInstance();
	const auto dInput = input->GetDirectInput();

	if (dInput->IsTrigger(DIK_I)) {
		SpawnSplash(Vector2{ 3.0f, 5.0f }, Vector2{ 0.0f, 1.0f }, false);
	}
	if (dInput->IsTrigger(DIK_K)) {
		SpawnSplash(Vector2{ 3.0f, 5.0f }, Vector2{ 0.0f, -1.0f }, false);
	}
	if (dInput->IsTrigger(DIK_L)) {
		SpawnSplash(Vector2{ 3.0f, 5.0f }, Vector2{ 1.0f, 0.0f }, false);
	}
	if (dInput->IsTrigger(DIK_J)) {
		SpawnSplash(Vector2{ 3.0f, 5.0f }, Vector2{ -1.0f, 0.0f }, false);
	}

	for (const auto& particle : splashParticles_) {
		particle->Update(deltaTime, -0.01f);
	}
}

void TD_10days::ParticleManager::Draw() {
	for (const auto& particle : splashParticles_) {
		particle->Draw();
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
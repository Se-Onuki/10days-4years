/// @file TitleScene.h
/// @brief タイトルシーン
/// @author ONUKI seiya
#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../Engine/ECS/World/NewWorld.h"
#include "../Engine/ECS/System/FunctionalSystem.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"

#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"

class TitleScene : public SolEngine::IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	void ApplyGlobalVariables();

	//テクスチャの設定や移動など
	void TextureSetting();

private:
	// 入力インスタンス
	SolEngine::Input *input_ = nullptr;
	// 音インスタンス
	SolEngine::Audio *audio_ = nullptr;
	// カメラマネージャー
	SolEngine::CameraManager *cameraManager_ = nullptr;

	ECS::World world_;
	ECS::SystemExecuter systemExecuter_;

	std::unique_ptr<Tex2DState> backGround_;

	//テクスチャの情報群
	std::vector<Tex2DState*> texDetas_;

	std::pair<int32_t, int32_t> angleMinMax_;
	std::pair<Vector2, Vector2> posMinMax_;

	//ランダムで変化する変数
	int32_t randAngle_ = 0;
	Vector2 randPos_ = {};

	// bgm
	SolEngine::Audio::SoundHandle soundA_;
};

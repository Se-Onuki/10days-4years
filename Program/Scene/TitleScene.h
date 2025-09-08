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
	//シーン限定のimguiの配置
	void Debug();

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

	//培養ポットのUV動かす値
	const float kUVMoveValue_ = 300.0f;
	//UVの最大値
	const float kUVMaxValue_ = 2700.0f;

	//培養ポットのUV動かす値
	const float kUVMoveValueWater_ = 900.0f;
	//UVの最大値
	const float kUVMaxValuePlayerMoved_ = 1500.0f;

	//UVの最大値
	const float kUVMaxValuePlayer_ = 3600.0f;
 
	//右のからのポット
	Vector2 nullPotRightUV_ = { 0.0f, 0.0f };
	//ふぐが入ったポット
	Vector2 playerPotUV_ = { 0.0f, 0.0f };
	//初期値が違うもの
	Vector2 nullPotLeftUV_ = { 300.0f, 0.0f };

	std::unique_ptr<SoLib::DeltaTimer> timer_ = nullptr;
	std::unique_ptr<SoLib::DeltaTimer> colorTimer_ = nullptr;
	//クリックした瞬間を感知
	bool isClicked_ = false;
	//ふぐが移動しきったのを感知
	bool isFishMoved_ = false;
	//魚が
	bool isFishOutSide_ = false;

	//ランダムで変化する変数
	int32_t randAngle_ = 0;
	Vector2 randPos_ = {};

	uint32_t buttomColor_ = 0xffffffff;

	float moveSpeed_ = 0.25f;
	float moveSpeedButtom_ = 0.5f;

	// bgm
	SolEngine::Audio::SoundHandle titleBGM_;
	// bgm
	SolEngine::Audio::SoundHandle decisionSE_;

};

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


class SelectScene : public SolEngine::IScene {
public:
	SelectScene();
	~SelectScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

private:
	void ApplyGlobalVariables();

	void Debug();
	//プレイヤーの操作など
	void PlayerMoving();

	//テクスチャの設定や移動など
	void TextureSetting();

	void BackGroundSetting();

private:
	// 入力インスタンス
	SolEngine::Input* input_ = nullptr;
	// 音インスタンス
	SolEngine::Audio* audio_ = nullptr;
	// カメラマネージャー
	SolEngine::CameraManager* cameraManager_ = nullptr;

	ECS::World world_;
	ECS::SystemExecuter systemExecuter_;
	//pivotの固定値
	const Vector2 kPivotValue_ = { 0.5f,0.5f };
	//扉の選択時の大きさ
	Vector2 selectScaleDoor_ = { 360.0f,360.0f };
	//扉のそれ以外の時の大きさ
	Vector2 defaultScaleDoor_ = { 250.0f,250.0f };
	//数字の選択時の大きさ
	Vector2 selectScaleNumber_ = { 240.0f,240.0f };
	//数字のそれ以外の時の大きさ
	Vector2 defaultScaleNumber_ = { 160.0f,160.0f };

	Vector2 lStick_{};

	static const int32_t kMaxStages_ = 6;
	//ステージごとの幅
	const int32_t kBaseMoveValue_ = 450;

	//培養ポットのUV動かす値
	const float kUVMoveValue_ = 300.0f;

	//ランダムで変化する変数
	int32_t randAngle_ = 0;
	Vector2 randPos_ = {};

	int32_t stageNum_ = 0;

	//中心座標
	int32_t basePos_ = 0;

	float changeSpeed_ = 0.3f;

	float moveSpeed_ = 0.25f;

	//扉
	std::array<std::unique_ptr<Tex2DState>, kMaxStages_> doors_;
	//番号
	std::array<std::unique_ptr<Tex2DState>, kMaxStages_> numbers_;

	//テクスチャの情報群
	std::vector<Tex2DState*> texDetas_;

	std::pair<int32_t, int32_t> angleMinMax_;
	std::pair<Vector2, Vector2> posMinMax_;

	std::unique_ptr<SoLib::DeltaTimer> timer_ = nullptr;
	std::unique_ptr<SoLib::DeltaTimer> colorTimer_ = nullptr;

	uint32_t buttomColor_ = 0xffffffff;

	float moveSpeedButtom_ = 0.5f;

	// bgm
	SolEngine::Audio::SoundHandle selectBGM_;
	SolEngine::Audio::SoundHandle stageSelectSE_;
	SolEngine::Audio::SoundHandle stageChangeSE_;
	SolEngine::Audio::SoundHandle sceneBackSE_;
	/*扉のイージング*/
	//扉のイージングのための変数
	float changeScaleSpeed_ = 0.05f;
	//扉のイージングのための値
	float changeScaleValue_ = 0.0f;
	//イージングの基礎値
	float moveT_ = 0.0f;
	//変化の幅
	Vector2 changeScaleRangeBefore_ = {};
	//変化の幅
	Vector2 changeScaleRangeAfter_ = {};
	//実際に代入する入れ物
	Vector2 doorScale_ = {};

	
	//イージングさせるかどうか
	bool isEaseDoor_ = false;

	/*背景変更関係*/
	float backGroundMoveSpeed_ = 0.25f;

	std::unique_ptr<Tex2DState> backGround_;

	std::unique_ptr<SoLib::DeltaTimer> backGroundTimer_ = nullptr;

	//変更後背景のUV
	Vector2 backGroundUV_ = { 0.0f, 0.0f };
	Vector2 backGroundUVScale_ = { 0.0f, 0.0f };
};


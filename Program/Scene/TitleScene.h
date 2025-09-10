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

#include "../Game/TD_10days/CircleFade.h"

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
	//培養ポットのUV動かす値
	const float kUVMovePlayerValue_ = 200.0f;
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
	//プレイヤー
	Vector2 playerUV_ = { 0.0f, 0.0f };
	//タイトルの動きの幅yだけmin,maxの順
	Vector2 titleTexMoveRange_ = { -10.0f,10.0f };
	//ポットのタイマー
	std::unique_ptr<SoLib::DeltaTimer> timer_ = nullptr;
	//プレイヤーアニメーションのタイマー
	std::unique_ptr<SoLib::DeltaTimer> playerAnimTimer_ = nullptr;
	//始めるUIのタイマー
	std::unique_ptr<SoLib::DeltaTimer> colorTimerStart_ = nullptr;
	//タイトルロゴのタイマー
	std::unique_ptr<SoLib::DeltaTimer> titleTexMoveTimer_ = nullptr;
	//きょろきょろ間隔のタイマー
	std::unique_ptr<SoLib::DeltaTimer> lookAroundMoveTimer_ = nullptr;
	//クリックした瞬間を感知
	bool isClicked_ = false;
	//ふぐが移動しきったのを感知
	bool isFishMoved_ = false;
	//魚がポットの外に出たかどうか
	bool isFishOutSide_ = false;
	//魚が地面についたかどうか
	bool isOnGround_ = false;
	//魚がきょろきょろしたら
	bool isLookAround_ = false;
	//一回目のアニメーションか
	bool isFirstAnimation_ = true;
	//左を向くかどうか
	bool isLookLeft_ = false;
	//ランダムで変化する変数
	int32_t randAngle_ = 0;
	Vector2 randPos_ = {};
	//振り向き回数上限
	int32_t lookAroundLimit_ = 4;
	//現在の振りむき回数
	int32_t lookAroundNum_ = 0;

	uint32_t buttomColor_ = 0xffffffff;
	uint32_t startTexColor_ = 0xffffffff;

	float moveSpeed_ = 0.25f;
	float moveSpeedPlayer_ = 0.25f;
	float playerAnimSpeed_ = 0.25f;

	float colorChangeSpeed_ = 0.05f;
	float colorChangeValue_ = 0.05f;

	float titleTexMoveTimeSpeed_ = 0.01f;
	float titleTexMoveSpeed_ = 0.05f;
	float titleTexMoveSpeedBase_ = 0.05f;
	float titleTexMoveValue_ = 0.05f;
	float moveT_ = 0.5f;

	float lookAroundDistance_ = 0.5f;

	/*playerの移動回り*/
 	float gravity_ = 49.0f;
	//Y軸移動力
	float jumpPower_ = 100.0f;
	//X軸移動力
	float movePower_ = 40.0f;
	//X軸移動力
	float dashPower_ = 400.0f;


	// 移動量
	Vector2 velocity_{};
	// 加速度
	Vector2 acceleration_{};

	//プレイヤーの座標
	Vector2 BasePlayerPos_ = { 250.0f,460.0f };
	//プレイヤーの座標
	Vector2 playerPos_ = {};
	Vector2 titleTexPos_ = {};

	// bgm
	SolEngine::Audio::SoundHandle titleBGM_;
	// bgm
	SolEngine::Audio::SoundHandle decisionSE_;

};

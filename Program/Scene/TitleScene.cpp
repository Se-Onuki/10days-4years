/// @file TitleScene.cpp
/// @brief タイトルシーン
/// @author ONUKI seiya
#include "TitleScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

#include "../Header/Object/Fade.h"
#include "GameScene.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/ECS/System/NewSystems.h"
#include "WaterDemoScene.h"

TitleScene::TitleScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	cameraManager_ = SolEngine::CameraManager::GetInstance();
	blockRender_ = BlockManager::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {
	// ライトの生成
	light_ = DirectionLight::Generate();
	blockRender_->Init(1024u);
	blockHandleRender_ = ModelHandleListManager::GetInstance();
	blockHandleRender_->Init(1024u);
	ModelManager::GetInstance()->CreateDefaultModel();

	sprite_ = Sprite::Generate(TextureManager::Load("UI/Title/TitleECS.dds"));

	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	ground_.Init();
	camera_.Init();

	camera_.translation_.y = 5.f;

	// bgmのロード
	soundA_ = audio_->LoadMP3("resources/Audio/BGM/TitleBGM.mp3");

	soundA_.Play(true, 0.1f);

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "check.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, &world_);

	levelDataManager->Destory(levelData);

	systemExecuter_.AddSystem<ECS::System::Par::CalcEulerTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelDrawer>();

	//各シーンの最初に入れる
	TextureEditor::GetInstance()->SetSceneId(SceneID::Title);
}

void TitleScene::OnExit() {
	audio_->StopAllWave();
}

void TitleScene::Update() {

	// キーボードの入力取得
	[[maybe_unused]] static const auto *const keyBoard = input_->GetDirectInput();

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	blockRender_->clear();
	blockHandleRender_->clear();

	camera_.rotation_ *= SoLib::MakeQuaternion(SoLib::Euler{ 0.f, 11.25_deg * deltaTime, 0.f });
	camera_.UpdateMatrix();

	sprite_->SetScale(Vector2{ 256,64 } *2.f);
	sprite_->SetPivot({ 0.5f,0.5f });
	
	sprite_->SetPosition(Vector2{ WinApp::kWindowWidth * 0.5f,WinApp::kWindowHeight * (1.f / 4.f) });

	if (input_->GetXInput()->IsTrigger(SolEngine::KeyCode::A) or input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		sceneManager_->ChangeScene<GameScene>(1.f);
		Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
	}

	if (input_->GetDirectInput()->IsTrigger(DIK_L)) {
		sceneManager_->ChangeScene<WaterDemoScene>(1.f);
		Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
	}

	// デルタタイムの取得
	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	systemExecuter_.Execute(&world_, deltaTime);
}

void TitleScene::Draw() {
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);

	light_->SetLight(commandList);
	// ground_.Draw();

	blockRender_->Draw(camera_);
	blockHandleRender_->Draw(camera_);

	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	sprite_->Draw();

	TextureEditor::GetInstance()->Draw();
	TextureEditor::GetInstance()->PutDraw();

	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}

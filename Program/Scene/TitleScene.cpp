/// @file TitleScene.cpp
/// @brief タイトルシーン
/// @author ONUKI seiya
#include "TitleScene.h"

#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

#include "../Header/Object/Fade.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/ECS/System/NewSystems.h"
#include <DirectBase/File/GlobalVariables.h>
#include <SelectScene.h>
#include "WaterDemoScene.h"

TitleScene::TitleScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	cameraManager_ = SolEngine::CameraManager::GetInstance();
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	const char* groupName = "UIRandom";
	global->CreateGroups(groupName);
	//アイテムの追加
	global->AddValue(groupName, "AngleRandomMin", angleMinMax_.first);
	global->AddValue(groupName, "AngleRandomMax", angleMinMax_.second);
	global->AddValue(groupName, "PosRandomMin", posMinMax_.first);
	global->AddValue(groupName, "PosRandomMax", posMinMax_.second);

	groupName = "PotAnim";
	global->CreateGroups(groupName);
	//アイテムの追加
	global->AddValue(groupName, "PotAnimationSpeed", moveSpeed_);


	// ライトの生成
	ModelManager::GetInstance()->CreateDefaultModel();

	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);
	timer_ = std::make_unique<SoLib::DeltaTimer>();
	timer_->Clear();
	colorTimer_ = std::make_unique<SoLib::DeltaTimer>();
	colorTimer_->Clear();

	// bgmのロード
	soundA_ = audio_->LoadMP3("resources/Audio/BGM/Title.mp3");

	soundA_.Play(true, 0.5f);

	SolEngine::ResourceObjectManager<SolEngine::LevelData> *const levelDataManager = SolEngine::ResourceObjectManager<SolEngine::LevelData>::GetInstance();

	auto levelData = levelDataManager->Load({ .fileName_ = "check.json" });

	SolEngine::LevelImporter levelImporter;
	levelImporter.Import(levelData, &world_);

	levelDataManager->Destory(levelData);

	systemExecuter_.AddSystem<ECS::System::Par::CalcEulerTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::CalcTransMatrix>();
	systemExecuter_.AddSystem<ECS::System::Par::ModelDrawer>();

	backGround_ = std::make_unique<Tex2DState>();
	backGround_->originalTransform.scale_ = { 1280.0f,720.0f };
	backGround_->originalTransform.translate_ = { 640.0f,360.0f };
	backGround_->sprite = Sprite::Generate(TextureManager::Load("TD_10days/BackGround/BackGround.png"));

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

	if (input_->GetXInput()->IsTrigger(SolEngine::KeyCode::A) or input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		sceneManager_->ChangeScene<SelectScene>(1.f);
		Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
	}
	ApplyGlobalVariables();

	TextureSetting();

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


	backGround_->sprite->Draw();

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);


	TextureEditor::GetInstance()->Draw();
	TextureEditor::GetInstance()->PutDraw();

	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}

void TitleScene::ApplyGlobalVariables(){
	GlobalVariables* global = GlobalVariables::GetInstance();
	const char* groupName = "UIRandom";

	angleMinMax_.first = global->Get<int>(groupName, "AngleRandomMin");
	angleMinMax_.second = global->Get<int>(groupName, "AngleRandomMax");
	posMinMax_.first = global->Get<Vector2>(groupName, "PosRandomMin");
	posMinMax_.second = global->Get<Vector2>(groupName, "PosRandomMax");

	groupName = "PotAnim";
	moveSpeed_ = global->Get<float>(groupName, "PotAnimationSpeed");
}

void TitleScene::TextureSetting(){
	backGround_->sprite->SetPosition(backGround_->originalTransform.translate_);
	backGround_->sprite->SetScale(backGround_->originalTransform.scale_);
	backGround_->sprite->SetPivot({ 0.5f,0.5f });

	texDetas_ = TextureEditor::GetInstance()->GetTitleTextures();
	
	randAngle_ = SoLib::Random::GetRandom(angleMinMax_.first, angleMinMax_.second);
	randPos_ = SoLib::Random::GetRandom(posMinMax_.first, posMinMax_.second);

	timer_->Update(ImGui::GetIO().DeltaTime);
	if (not timer_->IsActive()){
		playerPotUV_.x += kUVMoveValue_;
		nullPotLeftUV_.x += kUVMoveValue_;
		nullPotRightUV_.x += kUVMoveValue_;

		timer_->Clear();
		timer_->Start(moveSpeed_);
	}
	colorTimer_->Update(ImGui::GetIO().DeltaTime);
	if (not colorTimer_->IsActive()) {
		if (buttomColor_ == 0xffffffff){
			buttomColor_ = 0x00000000;
		}
		else {
			buttomColor_ = 0xffffffff;
		}

		colorTimer_->Clear();
		colorTimer_->Start(moveSpeedButtom_);
	}

	if (playerPotUV_.x >= kUVMaxValuePlayer_){
		playerPotUV_.x = 0;
	}
	if (nullPotLeftUV_.x >= kUVMaxValue_) {
		nullPotLeftUV_.x = 0;
	}
	if (nullPotRightUV_.x >= kUVMaxValue_) {
		nullPotRightUV_.x = 0;
	}

	for (size_t i = 0; i < texDetas_.size(); i++) {
		Tex2DState* nowTex = texDetas_[i];
		if (nowTex->textureName == "TitleStartUI") {
			nowTex->angle_degrees = randAngle_;
			nowTex->transform.rotate_ = DegreeToRadian(nowTex->angle_degrees);
			nowTex->transform.translate_ = nowTex->originalTransform.translate_ + randPos_;
		}
		if (nowTex->textureName == "PlayerInCultureSolution") {
			nowTex->uvTransform.translate_ = (playerPotUV_);
		}
		if (nowTex->textureName == "CultureSolution" and nowTex->originalTransform.translate_.x == -74.0f) {
			nowTex->uvTransform.translate_ = (nullPotLeftUV_);
		}
		if (nowTex->textureName == "CultureSolution" and nowTex->originalTransform.translate_.x == 500.0f) {
			nowTex->uvTransform.translate_ = (nullPotRightUV_);
		}
		if (nowTex->textureName == "AButtomUI") {
			nowTex->color = (buttomColor_);
		}
	}


}

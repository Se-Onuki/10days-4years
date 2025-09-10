#include "SelectScene.h"
#include <imgui.h>
#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/SoLib/SoLib.h"

#include "../Header/Object/Fade.h"
#include "../Engine/LevelEditor/LevelImporter.h"
#include "../Engine/ECS/System/NewSystems.h"
#include <DirectBase/File/GlobalVariables.h>
#include <GameScene.h>
#include <TitleScene.h>

#include<SelectToGame/SelectToGame.h>

SelectScene::SelectScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	cameraManager_ = SolEngine::CameraManager::GetInstance();
	
}

SelectScene::~SelectScene(){

}

void SelectScene::OnEnter(){
	// ライトの生成
	ModelManager::GetInstance()->CreateDefaultModel();

	//Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);

	GlobalVariables* global = GlobalVariables::GetInstance();
	const char* groupName = "Door";

	global->AddValue(groupName, "DoorChangeScaleAfter", changeScaleRangeAfter_);
	global->AddValue(groupName, "ChangeScaleSpeed", changeScaleSpeed_);

	groupName = "BackGround";

	global->AddValue(groupName, "AnimSpeed", backGroundMoveSpeed_);
	global->AddValue(groupName, "BackUVScale", backGroundUVScale_);
	

	TD_10days::CircleFade::GetInstance()->Start(1.5f, false);

	timer_ = std::make_unique<SoLib::DeltaTimer>();
	timer_->Clear();

	colorTimer_ = std::make_unique<SoLib::DeltaTimer>();
	colorTimer_->Clear();

	backGroundTimer_ = std::make_unique<SoLib::DeltaTimer>();
	backGroundTimer_->Clear();

	// bgmのロード
	selectBGM_ = audio_->LoadMP3("resources/Audio/BGM/StageSelect.mp3");

	selectBGM_.Play(true, 0.5f);

	stageSelectSE_ = audio_->LoadMP3("resources/Audio/SE/Scene/StageChoice.mp3");
	stageChangeSE_ = audio_->LoadMP3("resources/Audio/SE/Scene/CursolMove.mp3");
	sceneBackSE_ = audio_->LoadMP3("resources/Audio/SE/Scene/Back.mp3");
	
	stageNum_ = SelectToGame::GetInstance()->GetStageNum();

	backGround_ = std::make_unique<Tex2DState>();
	backGround_->originalTransform.scale_ = { 1280.0f,720.0f };
	backGround_->originalTransform.translate_ = { 640.0f,360.0f };
	backGround_->sprite = Sprite::Generate(TextureManager::Load("TD_10days/BackGround/BackGround.png"));

	for (int32_t i = 0; i < kMaxStages_; i++){
		doors_[i] = std::make_unique<Tex2DState>();
		doors_[i]->originalTransform.scale_ = { 250.0f,250.0f };
		doors_[i]->originalTransform.translate_ = { 640.0f + (kBaseMoveValue_ * i) , 350.0f };
		doors_[i]->transform.translate_ = doors_[i]->originalTransform.translate_;
		doors_[i]->sprite = Sprite::Generate(TextureManager::Load("UI/Select/StageUI.png"));
		numbers_[i] = std::make_unique<Tex2DState>();
		numbers_[i]->originalTransform.scale_ = { 160.0f,160.0f };
		numbers_[i]->originalTransform.translate_ = { 640.0f + (kBaseMoveValue_ * i) ,380.0f };
		numbers_[i]->transform.translate_ = numbers_[i]->originalTransform.translate_;
		numbers_[i]->uvTransform.scale_ = { 240.0f,240.0f };
		numbers_[i]->uvTransform.translate_ = { i * 240.0f,0.0f };
		numbers_[i]->sprite = Sprite::Generate(TextureManager::Load("UI/Select/StageNumbers.png"));
	}
	SelectToGame::GetInstance()->SetStageMax(kMaxStages_);

	particleManager_ = std::make_unique<TD_10days::ParticleManager>();
	particleManager_->Init();

	if (SelectToGame::GetInstance()->GetClearFlug()) {
		particleManager_->SpawnStar(20);
	}

	//各シーンの最初に入れる
	TextureEditor::GetInstance()->SetSceneId(SceneID::StageSelect);
}

void SelectScene::OnExit(){
	audio_->StopAllWave();
}

void SelectScene::Update(){
	// キーボードの入力取得
	[[maybe_unused]] static const auto* const keyBoard = input_->GetDirectInput();

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	if (input_->GetXInput()->IsTrigger(SolEngine::KeyCode::A) or input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {

		if (not TD_10days::CircleFade::GetInstance()->GetTimer()->IsActive()) {
			stageSelectSE_.Play(false, 0.5f);
			isEaseDoor_ = true;
			SelectToGame::GetInstance()->SetStageNum(stageNum_);
			sceneManager_->ChangeScene<GameScene>(1.0f);
			TD_10days::CircleFade::GetInstance()->Start(1.5f, true);
		}
	}

	if (input_->GetXInput()->IsTrigger(SolEngine::KeyCode::START) or input_->GetDirectInput()->IsTrigger(DIK_ESCAPE)) {
		if (not TD_10days::CircleFade::GetInstance()->GetTimer()->IsActive()) {
			sceneBackSE_.Play(false, 0.5f);
		}
		sceneManager_->ChangeScene<TitleScene>(1.0f);
		//Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
		TD_10days::CircleFade::GetInstance()->Start(1.5f, true);
	}
	
	ApplyGlobalVariables();

	PlayerMoving();

	TextureSetting();

	BackGroundSetting();

	particleManager_->Update(deltaTime);

	Debug();

	// デルタタイムの取得
	// const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	systemExecuter_.Execute(&world_, deltaTime);
}

void SelectScene::Draw(){
	DirectXCommon* const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画


	backGround_->sprite->Draw();

	particleManager_->Draw();

	for (int32_t i = 0; i < kMaxStages_; i++){
		doors_[i]->sprite->Draw();
		numbers_[i]->sprite->Draw();
	}

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

	//Fade::GetInstance()->Draw();

	TD_10days::CircleFade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion


}

void SelectScene::ApplyGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	const char* groupName = "UIRandom";

	angleMinMax_.first = global->Get<int>(groupName, "AngleRandomMin");
	angleMinMax_.second = global->Get<int>(groupName, "AngleRandomMax");
	posMinMax_.first = global->Get<Vector2>(groupName, "PosRandomMin");
	posMinMax_.second = global->Get<Vector2>(groupName, "PosRandomMax");
	groupName = "Door";
	changeScaleRangeAfter_ = global->Get<Vector2>(groupName, "DoorChangeScaleAfter");
	changeScaleSpeed_ = global->Get<float>(groupName, "ChangeScaleSpeed");

	groupName = "BackGround";
	backGroundMoveSpeed_ = global->Get<float>(groupName, "AnimSpeed");
	backGroundUVScale_ = global->Get<Vector2>(groupName, "BackUVScale");
}

void SelectScene::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("選択しているステージ");
	ImGui::SliderInt("ステージ番号", &stageNum_, 0, 14);

	ImGui::End();

#endif // _DEBUG

}

void SelectScene::PlayerMoving(){
	timer_->Update(ImGui::GetIO().DeltaTime);
	colorTimer_->Update(ImGui::GetIO().DeltaTime);

	basePos_ = stageNum_ * kBaseMoveValue_;
	if (TD_10days::CircleFade::GetInstance()->GetTimer()->IsActive()) {
		return;
	}

	if (timer_->IsActive()){
		return;
	}
	else {
		timer_->Clear();
	}

	lStick_ = (input_->GetXInput()->GetState()->stickL_);

	//右入力
	if (lStick_.x > 0.65f or input_->GetXInput()->IsPress(SolEngine::KeyCode::DPAD_RIGHT) or input_->GetDirectInput()->IsPress(DIK_RIGHT) or input_->GetDirectInput()->IsPress(DIK_D)) {
		if (stageNum_ < (kMaxStages_ - 1)) {
			stageChangeSE_.Play(false, 0.5f);
			stageNum_++;
			// タイマーの実行
			timer_->Start(moveSpeed_);

		}
		
	}
	//左入力
	else if (lStick_.x < -0.65f or input_->GetXInput()->IsPress(SolEngine::KeyCode::DPAD_LEFT) or input_->GetDirectInput()->IsPress(DIK_LEFT) or input_->GetDirectInput()->IsPress(DIK_A)) {
		if (stageNum_ > 0) {
			stageChangeSE_.Play(false, 0.5f);
			stageNum_--;
			// タイマーの実行
			timer_->Start(moveSpeed_);
		}
		
	}

	
}

void SelectScene::TextureSetting() {
	for (int32_t i = 0; i < kMaxStages_; i++){
		//選択しているステージに応じて移動
		doors_[i]->transform.translate_.x = SoLib::Lerp(doors_[i]->transform.translate_.x, doors_[i]->originalTransform.translate_.x - basePos_, changeSpeed_);
		numbers_[i]->transform.translate_.x = SoLib::Lerp(numbers_[i]->transform.translate_.x, numbers_[i]->originalTransform.translate_.x - basePos_, changeSpeed_);
		//選択しているものは大きく表示
		if (i == stageNum_){
			doors_[i]->transform.scale_ = SoLib::Lerp(doors_[i]->transform.scale_, selectScaleDoor_, changeSpeed_);
			numbers_[i]->transform.scale_ = SoLib::Lerp(numbers_[i]->transform.scale_, selectScaleNumber_, changeSpeed_);
			numbers_[i]->transform.translate_.y = SoLib::Lerp(numbers_[i]->transform.translate_.y, 400.0f, changeSpeed_);
			changeScaleRangeBefore_ = selectScaleDoor_;
			if (isEaseDoor_) {
				changeScaleValue_ += changeScaleSpeed_;

				moveT_ = SoLib::easeInBack(changeScaleValue_);

				if (moveT_ > 1.0f){
					moveT_ = 1.0f;
				}

				doors_[i]->transform.scale_ = SoLib::Lerp(changeScaleRangeBefore_, changeScaleRangeAfter_, moveT_);
				numbers_[i]->transform.scale_ = Vector2::zero;
			}
		}
		else {
			if (isEaseDoor_) {
				doors_[i]->transform.scale_ = Vector2::zero;
				numbers_[i]->transform.scale_ = Vector2::zero;

			}
			else {
				doors_[i]->transform.scale_ = SoLib::Lerp(doors_[i]->transform.scale_, defaultScaleDoor_, changeSpeed_);
				numbers_[i]->transform.scale_ = SoLib::Lerp(numbers_[i]->transform.scale_, defaultScaleNumber_, changeSpeed_);
				numbers_[i]->transform.translate_.y = SoLib::Lerp(numbers_[i]->transform.translate_.y, 380.0f, changeSpeed_);

			}
		}

		doors_[i]->sprite->SetPosition(doors_[i]->transform.translate_);
		doors_[i]->sprite->SetScale(doors_[i]->transform.scale_);
		doors_[i]->sprite->SetPivot(kPivotValue_);

		numbers_[i]->sprite->SetPosition(numbers_[i]->transform.translate_);
		numbers_[i]->sprite->SetScale(numbers_[i]->transform.scale_);
		numbers_[i]->sprite->SetTexOrigin(numbers_[i]->uvTransform.translate_);
		numbers_[i]->sprite->SetTexDiff(numbers_[i]->uvTransform.scale_);
		numbers_[i]->sprite->SetPivot(kPivotValue_);
	}

	backGround_->sprite->SetPosition(backGround_->originalTransform.translate_);
	backGround_->sprite->SetScale(backGround_->originalTransform.scale_);
	backGround_->sprite->SetPivot(kPivotValue_);

	randAngle_ = SoLib::Random::GetRandom(angleMinMax_.first, angleMinMax_.second);
	randPos_ = SoLib::Random::GetRandom(posMinMax_.first, posMinMax_.second);
	if (not colorTimer_->IsActive()) {
		if (buttomColor_ == 0xffffffff) {
			buttomColor_ = 0x00000000;
		}
		else {
			buttomColor_ = 0xffffffff;
		}

		colorTimer_->Clear();
		colorTimer_->Start(moveSpeedButtom_);
	}
	texDetas_ = TextureEditor::GetInstance()->GetSelectTextures();
	for (size_t i = 0; i < texDetas_.size(); i++) {
		Tex2DState* nowTex = texDetas_[i];
		if (nowTex->textureName == "AButtomUI") {
			if (isEaseDoor_){
				nowTex->transform.scale_ = Vector2::zero;
			}
			else {
				nowTex->transform.scale_ = nowTex->originalTransform.scale_;
			}
			nowTex->color = (buttomColor_);
		}

		if (nowTex->textureName == "EnterTheStageUI") {
			if (isEaseDoor_) {
				nowTex->transform.scale_ = Vector2::zero;
			}
			else {
				nowTex->transform.scale_ = nowTex->originalTransform.scale_;
			}
			nowTex->transform.rotate_ = DegreeToRadian(randAngle_);
			nowTex->transform.translate_ = nowTex->originalTransform.translate_ + randPos_;
		}

		if (nowTex->textureName == "StageSelectUI") {
			if (isEaseDoor_) {
				nowTex->transform.scale_ = Vector2::zero;
			}
			else {
				nowTex->transform.scale_ = nowTex->originalTransform.scale_;
			}
		}
		if (nowTex->textureName == "StartButtomUI") {
			if (isEaseDoor_) {
				nowTex->transform.scale_ = Vector2::zero;
			}
			else {
				nowTex->transform.scale_ = nowTex->originalTransform.scale_;
			}
		}
		if (nowTex->textureName == "GoTitleUI") {
			if (isEaseDoor_) {
				nowTex->transform.scale_ = Vector2::zero;
			}
			else {
				nowTex->transform.scale_ = nowTex->originalTransform.scale_;
			}
		}
	}
}

void SelectScene::BackGroundSetting(){
	bool clearFlug = SelectToGame::GetInstance()->GetClearFlug();
	//ステージ6をクリア
	if (clearFlug){

		
		//フェードが動いていないとき
		if (not TD_10days::CircleFade::GetInstance()->GetTimer()->IsActive()) {

			backGroundTimer_->Update(ImGui::GetIO().DeltaTime);

			if (not backGroundTimer_->IsActive()) {
				backGroundUV_.x += kUVMoveValue_;

				backGroundTimer_->Clear();
				backGroundTimer_->Start(backGroundMoveSpeed_);
			}
		}
		backGround_->sprite->SetTextureHaundle(TextureManager::Load("TD_10days/BackGround/ClearStageSelect.png"));
		backGround_->sprite->SetTexOrigin(backGroundUV_);
		backGround_->sprite->SetTexDiff(backGroundUVScale_);

		
	}
	

}

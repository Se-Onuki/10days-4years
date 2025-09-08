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
	global->AddValue(groupName, "PlayerAnimation", moveSpeedPlayer_);

	groupName = "TitlePlayerMove";
	global->CreateGroups(groupName);
	//アイテムの追加
	global->AddValue(groupName, "Gravity", gravity_);
	global->AddValue(groupName, "JumpPower", jumpPower_);
	global->AddValue(groupName, "MovePower", movePower_); 
	global->AddValue(groupName, "DashPower", dashPower_);
	// ライトの生成
	ModelManager::GetInstance()->CreateDefaultModel();

	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);
	timer_ = std::make_unique<SoLib::DeltaTimer>();
	timer_->Clear();
	playerAnimTimer_ = std::make_unique<SoLib::DeltaTimer>();
	playerAnimTimer_->Clear();
	colorTimer_ = std::make_unique<SoLib::DeltaTimer>();
	colorTimer_->Clear();

	// bgmのロード
	titleBGM_ = audio_->LoadMP3("resources/Audio/BGM/Title.mp3");
	titleBGM_.Play(true, 0.5f);

	decisionSE_ = audio_->LoadMP3("resources/Audio/SE/Scene/Choice.mp3");	

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

	playerPos_ = BasePlayerPos_;

	/*シーンが移動した際の巻き戻し用の処理*/
	texDetas_ = TextureEditor::GetInstance()->GetTitleTextures();
	for (size_t i = 0; i < texDetas_.size(); i++) {
		Tex2DState* nowTex = texDetas_[i];
		if (nowTex->textureName == "PlayerInCultureSolution") {

			nowTex->sprite->SetTextureHaundle((TextureManager::Load("UI/Title/PlayerInCultureSolution.png")));

			break;
		}
		if (nowTex->textureName == "PlayerWalk") {

			nowTex->transform.translate_ = playerPos_;
			nowTex->color = 0x00000000;
			playerUV_ = { 0.0f,0.0f };
			nowTex->uvTransform.translate_ = playerUV_;
		}
		
	}
}

void TitleScene::OnExit() {
	audio_->StopAllWave();
}

void TitleScene::Update() {

	// キーボードの入力取得
	[[maybe_unused]] static const auto *const keyBoard = input_->GetDirectInput();

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	if (input_->GetXInput()->IsTrigger(SolEngine::KeyCode::A) or input_->GetDirectInput()->IsTrigger(DIK_SPACE)) {
		if (not isFishOutSide_){
			isClicked_ = true;
			isOnGround_ = false;
			decisionSE_.Play(false, 0.5f);
			
			playerPotUV_ = { 0.0f,0.0f };
			isFishOutSide_ = true;
			velocity_ = Vector2::zero;
			velocity_.y -= jumpPower_;
			velocity_.x += movePower_;
			playerPos_ = BasePlayerPos_;
		}
		
	}
	else {
		if (not Fade::GetInstance()->GetTimer()->IsActive() and isFishMoved_){
			
			sceneManager_->ChangeScene<SelectScene>(1.f);
			Fade::GetInstance()->Start(Vector2{}, 0x000000FF, 1.f);
		}	
		isClicked_ = false;
	}
	playerAnimTimer_->Update(ImGui::GetIO().DeltaTime);

	//魚が外に出たら
	if (isFishOutSide_){
		if (not isOnGround_){
			//地面についていないとき
			acceleration_.y += gravity_ * deltaTime;

			velocity_ += acceleration_;
		}
		else {
			//地面についた後
			velocity_.y = 0;

			if (isLookAround_){
				/*きょろきょろし終わったら*/
				velocity_.x = dashPower_;

				if (not playerAnimTimer_->IsActive()) {
					playerUV_.x += kUVMovePlayerValue_;

					playerAnimTimer_->Clear();
					playerAnimTimer_->Start(moveSpeed_);
				}
			}
			else {
				/*きょろきょろする動作*/
				//タイマーによって切り替える
				
				if (not playerAnimTimer_->IsActive()) {
					playerUV_.x = 0;
					playerAnimTimer_->Clear();
				}
				else {
					playerUV_.x = kUVMovePlayerValue_;
				}

			}
		}

		Vector2 moveVec = velocity_ * deltaTime;

		playerPos_ += moveVec;

		if (playerPos_.y > 590) {
			//地面のテクスチャに近づいたら
			playerAnimTimer_->Start(moveSpeedPlayer_);
			isOnGround_ = true;
			velocity_.x = 0;
			playerPos_.y = 590;
		}

		// 加速度をリセット
		acceleration_ = Vector2::zero;
		//プレイヤーがある程度行ったら遷移させる
		if (playerPos_.x > 1400.0f){
			isFishMoved_ = true;
		}
	}
	

	Debug();

	ApplyGlobalVariables();

	TextureSetting();

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

void TitleScene::Debug() {

#ifdef _DEBUG
	ImGui::Begin("タイトルテスト用");

	ImGui::Checkbox("プレイヤーがボタンを押したかどうか", &isClicked_);
	ImGui::Checkbox("魚が移動しきったかどうか", &isFishMoved_);
	ImGui::Checkbox("きょろきょろし終わったかどうか", &isLookAround_);

	ImGui::End();
#endif // _DEBUG


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
	moveSpeedPlayer_ = global->Get<float>(groupName, "PlayerAnimation");

	groupName = "TitlePlayerMove";
	gravity_ = global->Get<float>(groupName, "Gravity");
	jumpPower_ = global->Get<float>(groupName, "JumpPower");
	movePower_ = global->Get<float>(groupName, "MovePower");
	dashPower_ = global->Get<float>(groupName, "DashPower");
}

void TitleScene::TextureSetting(){
	//背景
	backGround_->sprite->SetPosition(backGround_->originalTransform.translate_);
	backGround_->sprite->SetScale(backGround_->originalTransform.scale_);
	backGround_->sprite->SetPivot({ 0.5f,0.5f });

	
	randAngle_ = SoLib::Random::GetRandom(angleMinMax_.first, angleMinMax_.second);
	randPos_ = SoLib::Random::GetRandom(posMinMax_.first, posMinMax_.second);

	//テクスチャエディターの物
	texDetas_ = TextureEditor::GetInstance()->GetTitleTextures();
	for (size_t i = 0; i < texDetas_.size(); i++) {
		Tex2DState* nowTex = texDetas_[i];
		if (nowTex->textureName == "PlayerInCultureSolution") {
			if (isClicked_){
				nowTex->sprite->SetTextureHaundle((TextureManager::Load("UI/Title/BreakCultureSolution.png")));
			}
			nowTex->uvTransform.translate_ = (playerPotUV_);
		}
		if (nowTex->textureName == "CultureSolution" and nowTex->originalTransform.translate_.x == -74.0f) {
			nowTex->uvTransform.translate_ = (nullPotLeftUV_);
		}
		if (nowTex->textureName == "CultureSolution" and nowTex->originalTransform.translate_.x == 500.0f) {
			nowTex->uvTransform.translate_ = (nullPotRightUV_);
		}
		if (nowTex->textureName == "AButtomUI") {			
			if (isFishOutSide_){
				nowTex->color = 0x00000000;
			}
			else {
				nowTex->color = (buttomColor_);
			}
		}
		if (nowTex->textureName == "TitleStartUI") {
			nowTex->angle_degrees = randAngle_;
			nowTex->transform.rotate_ = DegreeToRadian(nowTex->angle_degrees);
			nowTex->transform.translate_ = nowTex->originalTransform.translate_ + randPos_;
			if (isFishOutSide_) {
				nowTex->color = 0x00000000;
			}
			else {
				nowTex->color = 0xffffffff;
			}
		}
		if (nowTex->textureName == "PlayerWalk") {
			if (isFishOutSide_){			
				nowTex->transform.translate_ = playerPos_;
				nowTex->color = 0xffffffff;
				nowTex->uvTransform.translate_ = playerUV_;
			}			
		}

	}
	//タイマーによって切り替える
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

	if (isFishOutSide_){
		if (playerPotUV_.x >= kUVMaxValuePlayerMoved_) {
			playerPotUV_.x = kUVMoveValueWater_;
		}
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



}

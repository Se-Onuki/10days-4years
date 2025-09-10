/// @file GameScene.cpp
/// @brief ゲームの処理を実装する
/// @author ONUKI seiya
#include "GameScene.h"

#include "ResultScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Object/Fade.h"
#include "../../Engine/Utils/SoLib/SoLib.h"
#include <imgui.h>
#include <utility>
#include "TitleScene.h"
#include "../../Engine/Utils/Math/Angle.h"
#include "../Engine/ResourceManager/ResourceLoader.h"

#include "../Engine/DirectBase/Base/TextureManager.h"
#include "SelectToGame/SelectToGame.h"

GameScene::GameScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
	stageEditor_ = StageEditor::GetInstance();
	auto bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();
	bufferManager->ReleaseUnusingReosurce();
}

GameScene::~GameScene() {
	auto bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();
	// メモリフラグメンテーション対策
	//for (uint32_t i = 1; i < 16; i++) {
	bufferManager->ReleaseUnusingReosurce();
	//}
}

void GameScene::OnEnter() {

	auto bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();
	bufferManager->ReleaseUnusingReosurce();

	pDxCommon_ = DirectXCommon::GetInstance();
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader, ShaderSource>::GetInstance();
	texStrage_ = SolEngine::FullScreenTextureStrage::GetInstance();

	SolEngine::Resource::ResourceLoadManager resourceLoadManager;
	SoLib::IO::File file{ "resources/Scene/GameScene.jsonc" };
	nlohmann::json sceneJson;
	file >> sceneJson;
	resourceLoadManager.Init(sceneJson["Resources"]);
	resourceLoadManager.Load();

	//Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);
	TD_10days::CircleFade::GetInstance()->Start(2.5f, false);


	offScreen_ = std::make_unique<PostEffect::OffScreenRenderer>();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ L"FullScreen.PS.hlsl", L"GrayScale.PS.hlsl", L"Vignetting.PS.hlsl",  L"Smoothing.PS.hlsl", L"GaussianFilter.PS.hlsl" ,  L"GaussianFilterLiner.PS.hlsl",  L"HsvFillter.PS.hlsl", L"WaterEffect.PS.hlsl", L"WhiteWaterEffect.PS.hlsl", L"Discard.PS.hlsl" });

	// bgmのロード
	gameBGM_ = audio_->LoadMP3("resources/Audio/BGM/Game.mp3");

	gameBGM_.Play(true, 0.5f);

	goalSE_ = audio_->LoadMP3("resources/Audio/SE/Scene/Clear.mp3");


	gaussianParam_->first = 32.f;
	gaussianParam_->second = 8;

	vignettingParam_ = { 16.f, 0.8f };

	background_ = Sprite::Generate(TextureManager::Load("TD_10days/BackGround/BackGround.png"));
	background_->SetScale(Vector2{ static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight) });
	background_->CalcBuffer();
	TextureEditor::GetInstance()->SetSceneId(SceneID::Game);

	camera_.Init();
	camera_.scale_ = SelectToGame::GetInstance()->GetCameraScale();
	camera_.translation_.y = 4.f;

	stageEditor_->Initialize(&levelMapChipRenderer_);

	pLevelMapChip_ = &(stageEditor_->GetMapChip());
	levelMapChipRenderer_.Init(pLevelMapChip_);
	pLevelMapChip_->CreateHitBox();
	levelMapChipHitBox_ = pLevelMapChip_->GetPlayerHitBox();
	levelMapChipWaterHitBox_ = pLevelMapChip_->GetWaterHitBox();
	

	player_.Init();
	player_.SetHitBox(levelMapChipHitBox_);
	player_.SetWaterHitBox(levelMapChipWaterHitBox_);

	
	water_ = std::make_unique<TD_10days::Water>();
	
	player_.SetWater(water_.get());

	// 念の為特殊なブロックの位置を再計算
	pLevelMapChip_->FindActionChips();
	// プレイヤのスタート位置の調整
	const Vector2 playerPos = pLevelMapChip_->GetStartPosition();
	// プレイヤの位置を設定
	player_.SetPosition(playerPos);

	auto [mapHeight, mapWidth] = pLevelMapChip_->GetSize();
	// スタート位置から初期のカメラ位置を決定
	startLine_.x = playerPos.x /*+ targetOffset_.x*/;
	startLine_.y = playerPos.y + targetOffset_.y;

	// ステージからスクロールを終了地点を決める
	endLine_.x = static_cast<float>(mapWidth - 1)  - stageOffset_.x;
	endLine_.y = static_cast<float>(mapHeight - 1) - stageOffset_.y;

	camera_.Init();
	camera_.scale_ = 0.015f;
	camera_.translation_ = Vector3{ startLine_.x + targetOffset_.x , startLine_.y, camera_.translation_.z };

	playerDrawer_ = std::make_unique<TD_10days::PlayerDrawer>();
	playerDrawer_->Init(&player_);

	// 水ブロック用パーティクルマネージャー
	waterParticleManager_ = std::make_unique<TD_10days::WaterParticleManager>();
	waterParticleManager_->Init();

	// パーティクルマネージャー
	particleManager_ = std::make_unique<TD_10days::ParticleManager>();
	particleManager_->Init(&camera_);

	water_->SetWaterParticleManager(waterParticleManager_.get());

	player_.SetParticleManager(particleManager_.get());

	//各シーンの最初に入れる
	TextureEditor::GetInstance()->SetSceneId(SceneID::Game);

}

void GameScene::OnExit() {
	audio_->StopAllWave();
	auto bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();
	bufferManager->ReleaseUnusingReosurce();

}

void GameScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);
	const float inGameDeltaTime = stageClearTimer_.IsActive() ? deltaTime * (1.f - stageClearTimer_.GetProgress()) : deltaTime;



	stageClearTimer_.Update(deltaTime);
	// もし範囲内で､タイマーが動いてないならスタート
	if (not stageClearTimer_.IsActive()) {
		// プレイヤの座標からゴールの距離を割り出す
		const Vector2 playerPos = player_.GetPosition();
		// ゴール座標からの距離で判定する
		for (const auto &goalPos : pLevelMapChip_->GetGoalPosition()) {
			if ((goalPos - playerPos).LengthSQ() < 1.f) {
				goalSE_.Play(false, 0.5f);
				stageClearTimer_.Start();

				stageTransitionFunc_ = (&GameScene::StageClear);
				break;
			}
		}

		const auto &needlePos = pLevelMapChip_->GetNeedlePosition();
		const Vector2 roundPos = Vector2{ std::roundf(playerPos.x), std::roundf(playerPos.y) };
		if (needlePos.find(roundPos) != needlePos.end()) {

			stageClearTimer_.Start();
			stageTransitionFunc_ = (&GameScene::StageDefeat);
		}
	}

	if (stageClearTimer_.IsActive() and stageClearTimer_.IsFinish()) {
		(this->*stageTransitionFunc_)();
	}

	// grayScaleParam_ = 1;
	Debug();

	ImGui::DragFloat2("VignettingParam", &vignettingParam_->first);

	ImGui::DragFloat("Sigma", &gaussianParam_->first);
	ImGui::DragInt("Size", &gaussianParam_->second);

	SoLib::ImGuiWidget("CameraPos", &camera_.translation_.ToVec2());
	SoLib::ImGuiWidget("CameraRot", &camera_.rotation_.z);
	SoLib::ImGuiWidget("CameraScale", &camera_.scale_);

	const Vector2 playerPosition = player_.GetPosition();

	// カメラ追従処理
	if (playerPosition.x > startLine_.x and playerPosition.x < endLine_.x) { // x方向
		camera_.translation_.x = playerPosition.x + targetOffset_.x;
	}
	else {
		// 範囲外 → 近い方のラインに固定
		float distToStart = std::abs(playerPosition.x - startLine_.x);
		float distToEnd = std::abs(playerPosition.x - endLine_.x);

		if (distToStart < distToEnd) {
			camera_.translation_.x = startLine_.x + targetOffset_.x;
		}
		else {
			camera_.translation_.x = endLine_.x + targetOffset_.x;
		}
	}

	//const auto hoge =(*pLevelMapChip_)[0][0];

	camera_.translation_.y = SoLib::Lerp(player_.GetPosition().y, camera_.translation_.y, 0.5f) + 1.0f;

	/*TD_10days::LevelMapChip::MapChipType mapChipType = (*pLevelMapChip_)[static_cast<int>(playerPosition.y + 4.0f)][static_cast<int>(playerPosition.x)];
	if (mapChipType == TD_10days::LevelMapChip::MapChipType::kEmpty) {
		camera_.translation_.y = player_.GetPosition().y;
	}*/
	
	//if (player_.GetPosition().y > startLine_.y and player_.GetPosition().y < endLine_.y) { // y方向
	//	camera_.translation_.y = player_.GetPosition().y;
	//}
	//else {
	//	// 範囲外 → 近い方のラインに固定
	//	float distToStart = std::abs(player_.GetPosition().y - startLine_.y);
	//	float distToEnd = std::abs(player_.GetPosition().y - endLine_.y);

	//	if (distToStart < distToEnd) {
	//		camera_.translation_.y = startLine_.y;
	//	}
	//	else {
	//		camera_.translation_.y = endLine_.y;
	//	}
	//}

	

	camera_.UpdateMatrix();

	// カメラから背景の位置を調整する
	const float cameraX = (camera_.translation_.x);
	background_->SetTexOrigin({ cameraX * 16.f, 0 });

	stageEditor_->SetCamera(camera_);
	stageEditor_->Update();

	SoLib::ImGuiWidget("PlayerPos", &player_.GetPosition());
	player_.PreUpdate(inGameDeltaTime);
	player_.InputFunc();
	player_.Update(inGameDeltaTime);
	playerDrawer_->Update(inGameDeltaTime);

	/*auto material = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance()->ImGuiWidget("MaterialManager");
	if (material) { SoLib::ImGuiWidget("Material", *material); }*/

	//SoLib::ImGuiWidget("HsvParam", hsvParam_.get());

	water_->Update(inGameDeltaTime);
	waterParticleManager_->Update(levelMapChipWaterHitBox_, 1.0f, deltaTime);

	particleManager_->Update(deltaTime);
}

void GameScene::Debug() {
#ifdef USE_IMGUI
	ImGuiIO &io = ImGui::GetIO();
	if (not ImGui::GetIO().WantCaptureMouse) {

		if (io.MouseWheel > 0.0f) {
			// ホイール上スクロール
			camera_.scale_ -= 0.01f;
		}
		if (io.MouseWheel < 0.0f) {
			// ホイール下スクロール
			camera_.scale_ += 0.01f;
		}
	}


#endif // USE_IMGUI

}

void GameScene::Draw() {

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	background_->Draw();


	Sprite::SetProjection(camera_.matView_ * camera_.matProjection_);

	particleManager_->DrawBack();

	stageEditor_->PutDraw();

	

	// スプライトの描画
	levelMapChipRenderer_.Draw();

	water_->Draw();

	playerDrawer_->Draw();

	DrawWater();

	particleManager_->Draw();

	

	player_.DrawUI();

	Sprite::SetDefaultProjection();

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);

	TextureEditor::GetInstance()->Draw();
	TextureEditor::GetInstance()->PutDraw();

	// スプライトの描画
	//Fade::GetInstance()->Draw();
	TD_10days::CircleFade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}

void GameScene::PostEffectSetup()
{
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxCommon_->GetDsvDescHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offScreen_->GetRtvDescHeap()->GetHeap()->GetCPUDescriptorHandleForHeapStart();

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	pDxCommon_->DrawTargetReset(&rtvHandle, offScreen_->GetClearColor(), &dsvHandle, viewport, scissorRect);


}

void GameScene::PostEffectEnd()
{

	auto *const postEffectProcessor = PostEffect::ShaderEffectProcessor::GetInstance();

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	auto resultTex = texStrage_->Allocate();

	// ポストエフェクトの初期値
	postEffectProcessor->Input(offScreen_->GetResource());

	//// ガウスぼかし
	//if (gaussianParam_->second > 1) {
	//	// 処理の実行
	//	postEffectProcessor->Execute(L"GaussianFilterLiner.PS.hlsl", gaussianParam_);
	//	postEffectProcessor->Execute(L"GaussianFilter.PS.hlsl", gaussianParam_);
	//}

	// postEffectProcessor->Execute(L"Vignetting.PS.hlsl", vignettingParam_);

	/*if (*grayScaleParam_.get() != 0) {
		postEffectProcessor->Execute(L"GrayScale.PS.hlsl", grayScaleParam_);
	}*/
	// postEffectProcessor->Execute(L"HsvFillter.PS.hlsl", hsvParam_);

	// 結果を取り出す
	postEffectProcessor->GetResult(resultTex->renderTargetTexture_.Get());


	pDxCommon_->DefaultDrawReset(false);

	fullScreen_->Draw({ L"FullScreen.PS.hlsl" }, **resultTex);

}

void GameScene::DrawWater()
{

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();

	auto resultTex = texStrage_->Allocate();

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxCommon_->GetDsvDescHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = resultTex->rtvHandle_.cpuHandle_;

#pragma region ViewportとScissor(シザー)

	// ビューポート
	D3D12_VIEWPORT viewport;
	// シザー短形
	D3D12_RECT scissorRect{};

	Sprite::EndDraw();

	pDxCommon_->SetFullscreenViewPort(&viewport, &scissorRect);

#pragma endregion

	pDxCommon_->DrawTargetReset(&rtvHandle, resultTex->clearColor_, &dsvHandle, viewport, scissorRect);


	Sprite::StartDraw(commandList);

	Sprite::SetProjection(camera_.matView_ * camera_.matProjection_);

	waterParticleManager_->Draw();

	Sprite::EndDraw();

	auto *const postEffectProcessor = PostEffect::ShaderEffectProcessor::GetInstance();
	// ポストエフェクトの初期値
	postEffectProcessor->Input(resultTex->renderTargetTexture_.Get());

	postEffectProcessor->Execute(L"WhiteWaterEffect.PS.hlsl");
	// ガウスぼかし
	if (gaussianParam_->second > 1) {
		// 処理の実行
		postEffectProcessor->Execute(L"GaussianFilterLiner.PS.hlsl", gaussianParam_);
		postEffectProcessor->Execute(L"GaussianFilter.PS.hlsl", gaussianParam_);
	}

	postEffectProcessor->Execute(L"WhiteWaterEffect.PS.hlsl");

	fullScreen_->Draw({ L"WaterEffect.PS.hlsl" }, **resultTex);

	// 結果を取り出す
	postEffectProcessor->GetResult(resultTex->renderTargetTexture_.Get());


	rtvHandle = offScreen_->GetRtvDescHeap()->GetHeap()->GetCPUDescriptorHandleForHeapStart();

	pDxCommon_->DrawTargetReset(&rtvHandle, offScreen_->GetClearColor(), &dsvHandle, viewport, scissorRect, false);

	fullScreen_->Draw({ L"Discard.PS.hlsl" }, **resultTex);

	Sprite::StartDraw(commandList);

	Sprite::SetProjection(camera_.matView_ * camera_.matProjection_);


}

void GameScene::StageClear()
{
	ResetStage(true);

}

void GameScene::StageDefeat()
{
	ResetStage(false);
}

void GameScene::ResetStage(bool isNext)
{
	// ステージ番号のマネージャ
	const auto levelSelecter = SelectToGame::GetInstance();
	// ステージ番号
	const auto stageNum = levelSelecter->GetStageNum();
	// ステージ番号を加算するかの分岐
	levelSelecter->SetStageNum(stageNum + (isNext ? 1 : 0));

	sceneManager_->ChangeScene("GameScene");
}

void GameScene::Load(const GlobalVariables::Group &)
{

}

void GameScene::Save(GlobalVariables::Group &) const
{

}

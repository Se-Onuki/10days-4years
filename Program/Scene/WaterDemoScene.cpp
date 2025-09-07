#include "WaterDemoScene.h"

#include "ResultScene.h"

#include "../Engine/DirectBase/Base/DirectXCommon.h"
#include "../Engine/DirectBase/Descriptor/DescriptorHandle.h"
#include "../Header/Object/Fade.h"
#include "../../Engine/Utils/SoLib/SoLib.h"
#include <imgui.h>
#include "TitleScene.h"
#include "../../Engine/Utils/Math/Angle.h"
#include "../Engine/ResourceManager/ResourceLoader.h"

#include "../Engine/DirectBase/Base/TextureManager.h"

WaterDemoScene::WaterDemoScene() {
	input_ = SolEngine::Input::GetInstance();
	audio_ = SolEngine::Audio::GetInstance();
}

WaterDemoScene::~WaterDemoScene() {
	auto bufferManager = SolEngine::DxResourceBufferPoolManager<>::GetInstance();
	// メモリフラグメンテーション対策
	for (uint32_t i = 1; i < 16; i++) {
		bufferManager->ReleaseUnusingReosurce(i);
	}
}

void WaterDemoScene::OnEnter() {

	pDxCommon_ = DirectXCommon::GetInstance();
	pShaderManager_ = SolEngine::ResourceObjectManager<Shader, ShaderSource>::GetInstance();
	texStrage_ = SolEngine::FullScreenTextureStrage::GetInstance();

	SolEngine::Resource::ResourceLoadManager resourceLoadManager;
	SoLib::IO::File file{ "resources/Scene/GameScene.jsonc" };
	nlohmann::json sceneJson;
	file >> sceneJson;
	resourceLoadManager.Init(sceneJson["Resources"]);
	resourceLoadManager.Load();

	Fade::GetInstance()->Start(Vector2{}, 0x00000000, 1.f);


	offScreen_ = std::make_unique<PostEffect::OffScreenRenderer>();
	offScreen_->Init();

	fullScreen_ = PostEffect::FullScreenRenderer::GetInstance();
	fullScreen_->Init({ L"FullScreen.PS.hlsl", L"GrayScale.PS.hlsl", L"Vignetting.PS.hlsl",  L"Smoothing.PS.hlsl", L"GaussianFilter.PS.hlsl" ,  L"GaussianFilterLiner.PS.hlsl",  L"HsvFillter.PS.hlsl", L"WaterEffect.PS.hlsl" });


	gaussianParam_->first = 32.f;
	gaussianParam_->second = 1;

	vignettingParam_ = { 16.f, 0.8f };


	levelMapChip_.Init(10, 10);
	levelMapChip_.SetMapChipData(
		{
		{},
		{ TextureHandle{TextureManager::Load("uvChecker.png")} },
		});
	std::fill(levelMapChip_[0].begin(), levelMapChip_[0].end(), TD_10days::LevelMapChip::MapChip::kWall);
	levelMapChip_[1][0] = TD_10days::LevelMapChip::MapChip::kWall;
	levelMapChip_[2][0] = TD_10days::LevelMapChip::MapChip::kWall;
	levelMapChip_[3][0] = TD_10days::LevelMapChip::MapChip::kWall;
	//levelMapChipRenderer_.Init(levelMapChip_);


	camera_.Init();

	//waterEffect_ = std::make_unique<TD_10days::WaterEffect>();
	//waterEffect_->Init();

	waterOffScreen_ = std::make_unique<PostEffect::OffScreenRenderer>();
	waterOffScreen_->Init();
}

void WaterDemoScene::OnExit() {
	audio_->StopAllWave();


}

void WaterDemoScene::Update() {

	[[maybe_unused]] const float deltaTime = std::clamp(ImGui::GetIO().DeltaTime, 0.f, 0.1f);

	// grayScaleParam_ = 1;

	ImGui::DragFloat2("VignettingParam", &vignettingParam_->first);

	ImGui::DragFloat("Sigma", &gaussianParam_->first);
	ImGui::DragInt("Size", &gaussianParam_->second);

	camera_.UpdateMatrix();
	SoLib::ImGuiWidget("CameraPos", &camera_.translation_);
	SoLib::ImGuiWidget("CameraRot", &camera_.rotation_.z);

	auto material = SolEngine::ResourceObjectManager<SolEngine::Material>::GetInstance()->ImGuiWidget("MaterialManager");
	if (material) { SoLib::ImGuiWidget("Material", *material); }

	SoLib::ImGuiWidget("HsvParam", hsvParam_.get());

	//waterEffect_->Update(/*deltaTime*/);
}

void WaterDemoScene::Draw() {

	DirectXCommon* const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* const commandList = dxCommon->GetCommandList();

#pragma region 背面スプライト

	Sprite::StartDraw(commandList);

	// スプライトの描画
	//levelMapChipRenderer_.Draw(camera_);

	Sprite::EndDraw();

#pragma endregion


	dxCommon->CrearDepthBuffer();

#pragma region モデル描画

	Model::StartDraw(commandList);


	Model::EndDraw();

#pragma endregion

#pragma region 前面スプライト

	Sprite::StartDraw(commandList);
	Sprite::SetBlendMode(Sprite::BlendMode::kNormal);

	//waterEffect_->Draw();
	// スプライトの描画
	Fade::GetInstance()->Draw();

	Sprite::EndDraw();

#pragma endregion

}

void WaterDemoScene::PostEffectSetup()
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

void WaterDemoScene::PostEffectEnd()
{

	auto* const postEffectProcessor = PostEffect::ShaderEffectProcessor::GetInstance();

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

	// ガウスぼかし
	if (gaussianParam_->second > 1) {
		// 処理の実行
		postEffectProcessor->Execute(L"GaussianFilterLiner.PS.hlsl", gaussianParam_);
		postEffectProcessor->Execute(L"GaussianFilter.PS.hlsl", gaussianParam_);
	}

	postEffectProcessor->Execute(L"Vignetting.PS.hlsl", vignettingParam_);

	if (*grayScaleParam_.get() != 0) {
		postEffectProcessor->Execute(L"GrayScale.PS.hlsl", grayScaleParam_);
	}
	postEffectProcessor->Execute(L"HsvFillter.PS.hlsl", hsvParam_);

	postEffectProcessor->Execute(L"WaterEffect.PS.hlsl");

	// 結果を取り出す
	postEffectProcessor->GetResult(resultTex->renderTargetTexture_.Get());


	pDxCommon_->DefaultDrawReset(false);

	fullScreen_->Draw({ L"FullScreen.PS.hlsl" }, resultTex->renderTargetTexture_.Get(), resultTex->srvHandle_.gpuHandle_);

}

void WaterDemoScene::Load(const GlobalVariables::Group&)
{

}

void WaterDemoScene::Save(GlobalVariables::Group&) const
{

}

/// @file GameScene.h
/// @brief ゲームシーンの実装
/// @author ONUKI seiya

#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraManager.h"

#include "../../Engine/Utils/Math/Transform.h"

#include "../Engine/DirectBase/Base/Audio.h"
#include "../Engine/DirectBase/Input/Input.h"

#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/IO/CSV.h"
#include "../../Header/Object/Particle.h"
#include "../Engine/DirectBase/DxResource/DxResourceBufferPool.h"
#include "../Engine/DirectBase/PostEffect/OffScreenRendering.h"
#include "../Engine/DirectBase/Render/ModelInstancingRender/ModelInstancingRender.h"
#include "../Engine/DirectBase/Texture/FullScreenTextureStrage.h"
#include "../Engine/ResourceObject/ResourceObjectManager.h"
#include "../Header/Entity/Entity.h"
#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"
#include "../Header/Object/HealthBar.h"
#include "../Header/Object/PlayerLevel/LevelUP.h"
#include "../Header/Object/Fade.h"
#include "../Game/Resource/GameScore.h"
#include "../Engine/UI/Text/NumberRender.h"
#include "../Engine/DirectBase/File/GlobalVariables.h"
#include "../Game/Resource/EnemyDataTable.h"
#include "../Engine/VFX/Particle/Particle.h"
#include "../Game/UI/ControllerUI.h"
#include <Editor/StageEditor.h>

#include "../Engine/DirectBase/Base/TextureManager.h"

#include "../Game/TD_10days/LevelMapChip.h"
#include "../Game/TD_10days/Object/Player.h"
#include "../Game/TD_10days/Object/Water.h"
#include "../Game/TD_10days/WaterEffect.h"
#include "../Game/UI/PlacementUI.h"
#include "../Game/TD_10days/Particle/ParticleManager.h"
#include "../Game/TD_10days/CountUI.h"


/// @class GameScene
/// @brief ゲームシーン
class GameScene : public SolEngine::IScene {
public:
	/// @fn GameScene(void)
	/// @brief コンストラクタ
	/// @details 各種リソースオブジェクトを初期化する
	GameScene();
	/// @fn ~GameScene(void)
	/// @brief デストラクタ
	/// @details 各種リソースオブジェクトの解放をする
	~GameScene();

	/// @fn void OnEnter(void)
	/// @brief シーンの初期化処理
	/// @details シーンの入室時に呼び出される
	void OnEnter() override;
	/// @fn void OnExit(void)
	/// @brief シーンの終了処理
	/// @details シーンの退室時に呼び出される
	void OnExit() override;

	/// @fn void Update(void)
	/// @brief シーンの更新処理
	/// @details シーンの更新時に呼び出される
	void Update() override;

	/// @fn void Draw(void)
	/// @brief シーンの描画処理
	/// @details シーンの描画時に呼び出される
	void Draw() override;

	/// @fn void PostEffectSetup(void)
	/// @brief ポストエフェクトの設定処理
	/// @details シーンの描画処理の前に呼び出される
	void PostEffectSetup() override;

	/// @fn void PostEffectEnd(void)
	/// @brief ポストエフェクトの終了処理
	/// @details シーンの描画処理の後に呼び出される
	void PostEffectEnd() override;


private:

	void DrawWater();

	/// @brief ステージクリア時の処理を実行します。
	void StageClear();

	/// @brief ステージでミスした場合の処理を実行します
	void StageDefeat();

	/// @brief ステージをリセットする
	void ResetStage(bool isNext);
	//imguiとかDebugでしか利用しないものまとめ
	void Debug();
private:

	void Load(const GlobalVariables::Group &group);

	void Save(GlobalVariables::Group &group) const;

private:

	SoLib::DeltaTimer stageClearTimer_{ 1.f, false };

	/// @brief ステージの遷移を行う関数
	void (GameScene:: *stageTransitionFunc_)() = (&GameScene::StageClear);

	/// @brief シェーダーのリソースマネージャ
	SolEngine::ResourceObjectManager<Shader> *pShaderManager_ = nullptr;
	/// @brief 入力のインスタンス
	SolEngine::Input *input_ = nullptr;
	/// @brief オーディオのインスタンス
	SolEngine::Audio *audio_ = nullptr;
	/// @brief DirectXの管理クラスのインスタンス
	DirectXCommon *pDxCommon_ = nullptr;

	/// @brief オフスクリーンレンダラの実体
	std::unique_ptr<PostEffect::OffScreenRenderer> offScreen_ = nullptr;
	/// @brief テクスチャストレージのインスタンス
	SolEngine::FullScreenTextureStrage *texStrage_ = nullptr;
	/// @brief フルスクリーンレンダラのインスタンス
	PostEffect::FullScreenRenderer *fullScreen_ = nullptr;

	/// @brief ヴィネッティングのパラメータ
	CBuffer<std::pair<float, float>> vignettingParam_{};
	/// @brief グレースケールのパラメータ
	CBuffer<float> grayScaleParam_{};
	/// @brief hsvの調整パラメータ
	CBuffer<SoLib::Color::HSV4> hsvParam_{ {0.f, 0.5f, 0.5f, 1.f} };
	/// @brief ガウシアンブラーのパラメータ
	CBuffer<std::pair<float, int32_t>> gaussianParam_{};

	// 影の色
	SoLib::Color::RGB4 shadowColor_ = 0x00000055;
	// 経験値の色
	SoLib::Color::RGB4 expColor_ = 0x555500FF;

	StageEditor *stageEditor_ = nullptr;

	TD_10days::LevelMapChip *pLevelMapChip_;
	TD_10days::LevelMapChipRenderer levelMapChipRenderer_;
	const TD_10days::LevelMapChip::LevelMapChipHitBox *levelMapChipHitBox_;
	const TD_10days::LevelMapChip::LevelMapChipHitBox *levelMapChipWaterHitBox_;

	SolEngine::Camera2D camera_;
	Vector2 startLine_{7.5f, 5.0f};
	Vector2 endLine_{};
	Vector2 halfWindowSize_{640.0f, 360.0f};

	TD_10days::Player player_;

	std::unique_ptr<Sprite> background_ = nullptr;

	std::unique_ptr<TD_10days::Water> water_;

	std::unique_ptr<TD_10days::WaterParticleManager> waterParticleManager_ = nullptr;
	std::unique_ptr<TD_10days::ParticleManager> particleManager_;

	// bgm
	SolEngine::Audio::SoundHandle gameBGM_;
	SolEngine::Audio::SoundHandle goalSE_;

};
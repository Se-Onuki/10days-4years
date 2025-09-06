#pragma once
#include<TD_10days/LevelMapChip.h>
#include<../User/AoMidori.h>
#include "Utils/Containers/Singleton.h"

/*マップチップのステージを制作するためのエディター*/
class StageEditor : public SoLib::Singleton<StageEditor> {
public:
	StageEditor() = default;
	StageEditor(const StageEditor &) = delete;
	StageEditor(StageEditor &&) = delete;
	~StageEditor();

	StageEditor &operator=(const StageEditor &) = delete;
	StageEditor &operator=(StageEditor &&) = delete;

	friend SoLib::Singleton<StageEditor>;
public:

	/// @brief マップチップのデータの確定
	void ApplyMapChips();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(TD_10days::LevelMapChipRenderer *pLevelMapChipRender);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 設置位置描画処理
	/// </summary>
	void PutDraw();

	/// <summary>
	/// Imguiの情報
	/// </summary>
	void Debug(Vector2 mousePos);


	//全てのファイルを読み込む
	void LoadFileAll();

	//設定したMapchipを返す
	TD_10days::LevelMapChip &GetMapChip() {
		return levelMapChip_;
	}

	//カメラの位置を外部から取得する
	void SetCamera(const SolEngine::Camera2D &camera) {
		camera_ = camera;
	}

private:
	/// @brief 一度しか呼び出さない初期化処理
	void InitOnce();

	/// <summary>
	/// クリックしたときと押し続けているときの動き
	/// </summary>
	void ClickPushMove(Vector2 mousePos);
	/// <summary>
	/// ドラッグしたときの挙動
	/// </summary>
	/// <param name="id">シーンのID</param>
	/// <param name="mousePos">マウスのポジション</param>
	void DragMove();
	//intをマップの対応したものに変換
	TD_10days::LevelMapChip::MapChip NumberToMap(const int32_t num);

	//intをマップの対応したものに変換
	int32_t MapToNumber(const TD_10days::LevelMapChip::MapChip map);

	void LoadStage();

	SoLib::Angle::Radian DegreeToRadian(int32_t degree) {
		return SoLib::Angle::Radian(degree * SoLib::Angle::Dig2Rad);
	}

public:
	static const float scaleMoveSpeed;


private:

	TD_10days::LevelMapChipRenderer *pLevelMapChipRender_ = nullptr;
	//マップチップ
	TD_10days::LevelMapChip levelMapChip_;
	//当たり判定用のカメラ
	SolEngine::Camera2D camera_;
	//配置場所可視化用テクスチャ
	std::unique_ptr<Tex2DState> newTex_;
	//テクスチャのパス
	std::vector<std::string> texPath_;
	//テクスチャの種類の名前
	std::vector<std::string> texName_;

	//保存した後に読み取れるように
	SoLib::IO::CSV csvData_;
	//保存ファイル
	SoLib::IO::File csvFile_;

	int blockSize_ = 40;

	std::pair<int32_t, int32_t> tilePos_;

	std::pair<int32_t, int32_t> mapSize_ = { 10,50 };

	std::pair<int32_t, int32_t> nowMapSize_ = { 10,50 };

	//ステージの最大値
	const int32_t kStageMax_ = 15;

	//Guiで選択している数
	int32_t guiSelectNum_ = 1;

	//選択しているステージ
	int32_t stageNum_ = -1;

	bool isIncide_ = false;

	bool isSave_ = false;

	bool isNotChangeRange_ = false;
	//UIエディターを使うかどうか
	bool isTextureEditor_ = true;

	int32_t selectNumber_ = 1;
	int32_t selectTexNumber_ = -1;
	//imgui用のナンバー
	int32_t selectTexImguiNumber_ = -1;

	const Vector2 kPivotValue_ = { 0.5f,0.5f };

	std::filesystem::path dragSelectPath_;

	/*ファイル制御関連*/
private:
	//メンバ関数
	using json = nlohmann::json;
	//ファイルに保存する
	void SaveFile(const std::string &fileName);
	//ファイルが存在するか確認する
	void ChackFiles();

	//ファイルを読み込む
	void LoadFile(const std::string &fileName);

	void LoadFiles(const std::string &fileName);

	//ファイルが存在するか確認する(指定)
	bool LoadChackItem(const std::string &fileName);

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation(const std::wstring text);

private:

	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Resources/Datas/StageData/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Resources/Datas/StageData";
	//名前
	inline static const std::string kItemName_ = "Stage";

	//保存するファイルの名前
	inline static const std::string kFileName_ = "StageMap";

	std::vector<std::string> fileName_;


};


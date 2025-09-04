#pragma once
#include<TD_10days/LevelMapChip.h>
#include<../User/AoMidori.h>

/*マップチップのステージを制作するためのエディター*/
class StageEditor{
public:
	StageEditor() = default;
	StageEditor(const StageEditor&) = delete;
	StageEditor(StageEditor&&) = delete;
	~StageEditor();

	StageEditor& operator=(const StageEditor&) = delete;
	StageEditor& operator=(StageEditor&&) = delete;
public:


	static StageEditor* const GetInstance() {
		static StageEditor instance;
		return &instance;
	}


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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
	const TD_10days::LevelMapChip& GetMapChip() {
		return levelMapChip_;
	}

	//カメラの位置を外部から取得する
	void SetCamera(const SolEngine::Camera2D& camera) {
		camera_ = camera;
	}
	
private:
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

	SoLib::Angle::Radian DegreeToRadian(int32_t degree) {
		return SoLib::Angle::Radian(degree * (std::numbers::pi_v<float> / 180.0f));
	}

public:
	static const float scaleMoveSpeed;


private:
	//マップチップ
	TD_10days::LevelMapChip levelMapChip_;
	//当たり判定用のカメラ
	SolEngine::Camera2D camera_;
	//配置場所可視化用テクスチャ
	std::unique_ptr<Tex2DState> newTex_;

	const int blockSize_ = 40;

	std::pair<int32_t, int32_t> tilePos_;

	std::pair<int32_t, int32_t> mapSize_ = { 10,50 };

	bool isIncide_ = false;

	bool isSelectFlug_ = false;
	//設置場所を決めるためのフラグ
	bool isDecideToPlace_ = true;
	//選択機能
	bool isSelecteTex_ = true;
	//クリックにするか
	bool isTriger_ = false;
	//長押しにするか
	bool isPush_ = true;
	//アイテムをつかむ
	bool isGrab_ = false;
	//ショートカットキーを利用するか
	bool isShortCuts_ = false;
	//テクスチャの元のサイズを使うかどうか
	bool isUseTextureBaseSize_ = true;
	//ドラッグをしているか
	bool isDraging_ = false;

	//UIエディターを使うかどうか
	bool isTextureEditor_ = true;

	int32_t selectNumber_ = -1;
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
	void SaveFile(const std::string& fileName);
	//ファイルが存在するか確認する
	void ChackFiles();

	//ファイルを読み込む
	void LoadFile(const std::string& fileName);

	void LoadFiles(const std::string& fileName);

	//ファイルが存在するか確認する(指定)
	bool LoadChackItem(const std::string& fileName);

	//imguiの操作をそのまま続けるかどうかのメッセージボックスを表示
	bool OperationConfirmation();

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Datas/StageData/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Datas/StageData";
	//名前
	inline static const std::string kItemName_ = "Stage";

	std::vector<std::string> fileName_;


};


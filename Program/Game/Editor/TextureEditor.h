#pragma once
//UIなどを配置できるエディター
#include<DirectBase/2D/Sprite.h>
#include<Collider/UICollider.h>

//UIを配置すためのエディター
//基本操作はマウスを中心に操作する
class TextureEditor{
public:
	TextureEditor() = default;
	TextureEditor(const TextureEditor&) = delete;
	TextureEditor(TextureEditor&&) = delete;
	~TextureEditor();

	TextureEditor& operator=(const TextureEditor&) = delete;
	TextureEditor& operator=(TextureEditor&&) = delete;
public:
	

	static TextureEditor* const GetInstance() {
		static TextureEditor instance;
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
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 設置位置描画処理
	/// </summary>
	void PutDraw();

	/// <summary>
	/// Imguiの情報
	/// </summary>
	void Debug(const SceneID id, Vector2 mousePos);


	//全てのファイルを読み込む
	void LoadFileAll();

	/*シーンごとのテクスチャを取得する関数*/

	std::vector<Tex2DState*> GetTitleTextures() {
		std::vector<Tex2DState*> texStateTitle;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[static_cast<size_t>(SceneID::Title)]) {
			texStateTitle.push_back(ptr.get());
		}
		return texStateTitle;
	}

	std::vector<Tex2DState*> GetGameTextures() {
		std::vector<Tex2DState*> texStateGame;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[static_cast<size_t>(SceneID::Game)]) {
			texStateGame.push_back(ptr.get());
		}
		return texStateGame;
	}

	std::vector<Tex2DState*> GetSelectTextures() {
		std::vector<Tex2DState*> texStateSelect;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[static_cast<size_t>(SceneID::StageSelect)]) {
			texStateSelect.push_back(ptr.get());
		}
		return texStateSelect;
	}

	std::vector<Tex2DState*> GetResultTextures() {
		std::vector<Tex2DState*> texStateResult;
		for (std::unique_ptr<Tex2DState>& ptr : texies_[static_cast<size_t>(SceneID::Result)]) {
			texStateResult.push_back(ptr.get());
		}
		return texStateResult;
	}
	/// <summary>
	/// シーンの最初に入れ、シーンごとの描画を変更
	/// </summary>
	/// <param name="id">SceneID::での物を入れる</param>
	void SetSceneId(const SceneID id) {
		id_ = id;
	}
private:
	/// <summary>
	/// クリックしたときと押し続けているときの動き
	/// </summary>
	void ClickPushMove(const SceneID id, Vector2 mousePos);
	/*/// <summary>
	/// ショートカットキーによる動作
	/// </summary>
	/// <param name="id">シーンのid</param>
	void ShortCutMove(const SceneID id);*/


	void NewTexMake(const SceneID id, std::filesystem::path path);

	void ChangeTex(std::filesystem::path path, Tex2DState* tex);
	/// <summary>
	/// ドラッグしたときの挙動
	/// </summary>
	/// <param name="id">シーンのID</param>
	/// <param name="mousePos">マウスのポジション</param>
	void DragMove(std::filesystem::path path);

	/// <summary>
	/// ドラッグしたときの挙動
	/// </summary>
	/// <param name="id">シーンのID</param>
	/// <param name="mousePos">マウスのポジション</param>
	void DropMove(const SceneID id, Vector2 mousePos);

public:
	static const float scaleMoveSpeed;


private:
	std::unordered_map<SceneID, std::string> sceneName_;

	/// <summary>
	/// 一時保存するリスト
	/// </summary>
	std::array<std::vector<std::unique_ptr<Tex2DState>>, static_cast<uint32_t>(SceneID::kNum)> texies_;

	/// <summary>
	/// 元の色を一時保存
	/// </summary>
	std::array<std::vector<uint32_t>, static_cast<uint32_t>(SceneID::kNum)> texColors_;

	std::unique_ptr<Tex2DState> newTex_;

	//スケールイージングするための
	bool isScaleMove_ = false;

	bool isScaleMoveReverse_ = false;

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

	float time_ = 0;

	int32_t selectNumber_ = -1;
	int32_t selectTexNumber_ = -1;
	//imgui用のナンバー
	int32_t selectTexImguiNumber_ = -1;

	/*std::unique_ptr<Easing> easing_;*/

	const Vector2 kPivotValue_ = { 0.5f,0.5f };

	Matrix4x4 camera_;

	SceneID id_;

	/*BlendType baseBlend_ = BlendType::kNone;*/
	
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
	void from_json(const json& j, Vector2& v);

	//jsonの数値をVector3に変換する関数
	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	//ファイルがちゃんと読み込めたかどうか
	bool chackOnlyNumber_ = 0;
	//ステージ名をいれるコンテナ
	std::vector<std::string> stages_;
	//選んでいるステージ名
	std::string stageName_;
	//アイテムのファイルパス
	inline static const std::string kDirectoryPath_ = "Datas/UIData/";
	//アイテムのファイルパス
	inline static const std::string kDirectoryName_ = "Datas/UIData";
	//名前
	inline static const std::string kItemName_ = "UIs";

	std::vector<std::string> fileName_;

};


#include "StageEditor.h"

StageEditor::~StageEditor(){

}

void StageEditor::Initialize() {
	levelMapChip_.Init(mapSize_.first, mapSize_.second);
	levelMapChip_.SetMapChipData(
		{
		{},
		{TextureHandle{TextureManager::Load("uvChecker.png")}},//wall
		{TextureHandle{TextureManager::Load("StageTex/blue.png")}},//tile
		{TextureHandle{TextureManager::Load("StageTex/green.png")}},//floor
		{TextureHandle{TextureManager::Load("StageTex/pink.png")}},//water
		{TextureHandle{TextureManager::Load("StageTex/red.png")}},//start
		{TextureHandle{TextureManager::Load("StageTex/yellow.png")}},//goal
		});
	std::fill(levelMapChip_[0].begin(), levelMapChip_[0].end(), TD_10days::LevelMapChip::MapChip::kFloor);
	levelMapChip_[1][0] = TD_10days::LevelMapChip::MapChip::kWall;
	levelMapChip_[1][2] = TD_10days::LevelMapChip::MapChip::kWall;
	levelMapChip_[2][0] = TD_10days::LevelMapChip::MapChip::kWall;
	levelMapChip_[3][0] = TD_10days::LevelMapChip::MapChip::kWall;

	newTex_ = std::make_unique<Tex2DState>();
	newTex_->transform.translate_ = Vector2(0, 0);
	newTex_->transform.scale_ = { 1.f,1.f };
	newTex_->color = 0x0000ffff;
	//配置用のものなのでwhiteTex
	newTex_->sprite = Sprite::Generate(TextureManager::LoadDefaultTexture());


}

void StageEditor::Finalize() {

}

void StageEditor::Update() {
	//マウスの座標をアプリと合わせる
	Vector2 mousePos = Vector2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	mousePos.y *= -1; // Y反転（上が正になる）
	mousePos += Vector2{ -640, 360 };

	//カメラを考慮した座標に変換
	std::pair<int32_t, int32_t> world{};
	world.first = (int32_t)(mousePos.x + (camera_.translation_.x * blockSize_));
	world.second = (int32_t)(mousePos.y + (camera_.translation_.y * blockSize_));


	// ---- マップチップの範囲内かどうか ----
	isIncide_ =
		((-19 <= world.first) and (world.first < (mapSize_.second * blockSize_))) and
		((-19 <= world.second) and (world.second < (mapSize_.first * blockSize_)));

	if (isIncide_) {
		// ---- どのマップチップか ----
		tilePos_.first = (world.first + 20) / blockSize_;
		tilePos_.second = (world.second + 20) / blockSize_;
		newTex_->transform.translate_ = { (float)(tilePos_.first),(float)(tilePos_.second) };
	}

	if (isDecideToPlace_) {
		//左クリックしたら
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			//マウスの位置に合わせる
			newTex_->transform.translate_ = mousePos;
		}
	}

	Debug(Vector2({ (float)(world.first),(float)(world.second) }));

	ClickPushMove(mousePos);
}

void StageEditor::PutDraw() {
	if (isIncide_){
		newTex_->sprite->SetColor(newTex_->color);
		newTex_->sprite->SetPosition(newTex_->transform.translate_);
		newTex_->sprite->SetScale(newTex_->transform.scale_);
		newTex_->sprite->SetRotate(newTex_->transform.rotate_.Get());
		newTex_->sprite->SetPivot(kPivotValue_);
		newTex_->sprite->SetInvertY(true);
		newTex_->sprite->Draw();
	}
}

void StageEditor::Debug(Vector2 mousePos) {
	ImGui::Begin("マウスの位置");
	ImGui::DragFloat2("座標", mousePos.data(), 0.1f);
	ImGui::DragInt("マップチップX", &tilePos_.first);
	ImGui::DragInt("マップチップY", &tilePos_.second);
	ImGui::End();
}

void StageEditor::LoadFileAll() {

}

void StageEditor::ClickPushMove(Vector2 mousePos){
	mousePos;
}

void StageEditor::SaveFile(const std::string& fileName) {
	fileName;
}

void StageEditor::ChackFiles() {
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::wstring message = L"Failed open data file for write.";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ディレクトリないよぉ！", 0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (LoadChackItem(filePath.stem().string())) {
			chackOnlyNumber_ = 1;
		}

		if (fileName_.size() != 0) {
			bool noneFail = true;
			for (size_t i = 0; i < fileName_.size(); i++) {
				if (fileName_[i].c_str() == filePath.stem().string()) {
					noneFail = false;
				}
			}
			if (noneFail) {
				fileName_.push_back(filePath.stem().string());
			}

		}
		else {
			//ファイルの名前を取得
			fileName_.push_back(filePath.stem().string());
		}
	}
}

void StageEditor::LoadFile(const std::string& fileName) {
#ifdef _DEBUG
	fileName;
	std::string message = "File loading completed";
	MessageBoxA(WinApp::GetInstance()->GetHWND(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

void StageEditor::DragMove() {
#ifdef _DEBUG
	
#endif
}

bool StageEditor::LoadChackItem(const std::string& fileName) {
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = fileName;
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WinApp::GetInstance()->GetHWND(), message.c_str(), "Object", 0);
		ifs.close();
		return false;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName_);
	//未登録チェック
	if (itGroup != root.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool StageEditor::OperationConfirmation() {
	int result = MessageBox(WinApp::GetInstance()->GetHWND(), L"この操作を続けますか?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES) {
		return true;
	}
	else if (result == IDNO) {
		return false;
	}
	else {
		return false;
	}
}

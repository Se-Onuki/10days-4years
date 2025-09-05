#include "StageEditor.h"
#include <SelectToGame/SelectToGame.h>

StageEditor::~StageEditor() {

}

void StageEditor::ApplyMapChips()
{
	levelMapChip_.CreateHitBox();
	levelMapChip_.FindActionChips();
}

void StageEditor::Initialize() {
	//ここにパスを入れる
	texName_ = {
		{},
		{("wall")},//wall
		{("tile")},//tile
		{("floor")},//floor
		{("water")},//water
		{("start")},//start
		{("goal")},//goal
	};

	texPath_ = {
		{},
		{("uvChecker.png")},//wall
		{("StageTex/blue.png")},//tile
		{("StageTex/green.png")},//floor
		{("StageTex/pink.png")},//water
		{("StageTex/red.png")},//start
		{("StageTex/yellow.png")},//goal
	};

	stageNum_ = SelectToGame::GetInstance()->GetStageNum();

	int32_t selectNum = stageNum_ + 1;

	if (csvFile_.Load(kDirectoryPath_ + kFileName_ + std::to_string(selectNum).c_str() + ".csv")) {
		csvData_ = csvFile_;

		levelMapChip_.Init(csvData_);
	}
	else {
		levelMapChip_.Init(mapSize_.first, mapSize_.second);
	}

	levelMapChip_.SetMapChipData(
		{
		{false},
		{TextureHandle{TextureManager::Load(texPath_[1])}},//wall
		{TextureHandle{TextureManager::Load(texPath_[2])}},//tile
		{TextureHandle{TextureManager::Load(texPath_[3])}},//floor
		{TextureHandle{TextureManager::Load(texPath_[4])}},//water
		{TextureHandle{TextureManager::Load(texPath_[5])}, false},//start
		{TextureHandle{TextureManager::Load(texPath_[6])}, false},//goal
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
	//カメラの距離に合わせてサイズを変更
	blockSize_ = (int)(1.0f / camera_.scale_);

	//カメラを考慮した座標に変換
	std::pair<int32_t, int32_t> world{};
	world.first = (int32_t)(mousePos.x + (camera_.translation_.x * blockSize_));
	world.second = (int32_t)(mousePos.y + (camera_.translation_.y * blockSize_));

	int blockQuater = (blockSize_ / 4);
	// ---- マップチップの範囲内かどうか ----
	isIncide_ =
		((-(blockQuater * 2) <= world.first) and (world.first < (mapSize_.second * blockSize_) - (blockQuater * 3))) and
		((-(blockQuater * 2) <= world.second) and (world.second < (mapSize_.first * blockSize_) - (blockQuater * 3)));

	if (not ImGui::GetIO().WantCaptureMouse) {
		if (isIncide_) {
			// ---- どのマップチップか ----
			tilePos_.first = (world.first + (blockSize_ / 2)) / blockSize_;
			tilePos_.second = (world.second + (blockSize_ / 2)) / blockSize_;


			newTex_->transform.translate_ = { (float)(tilePos_.first),(float)(tilePos_.second) };

			//左クリックしたら
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				levelMapChip_[tilePos_.second][tilePos_.first] = NumberToMap(selectNumber_);
				isSave_ = false;
			}
			//右クリックしたら
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
				levelMapChip_[tilePos_.second][tilePos_.first] = NumberToMap(0);
				isSave_ = false;
			}

		}
		//離した時に当たり判定の更新
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) or ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			ApplyMapChips();
		}
	}


	Debug(Vector2({ (float)(world.first),(float)(world.second) }));

	ClickPushMove(mousePos);
}

void StageEditor::PutDraw() {
	if (isIncide_) {
		newTex_->sprite->SetColor(newTex_->color);
		newTex_->sprite->SetPosition(newTex_->transform.translate_);
		newTex_->sprite->SetScale(newTex_->transform.scale_);
		newTex_->sprite->SetRotate(newTex_->transform.rotate_.Get());
		newTex_->sprite->SetPivot(kPivotValue_);
		newTex_->sprite->SetInvertY(true);
		newTex_->sprite->Draw();
	}
}

void StageEditor::Debug([[maybe_unused]] Vector2 mousePos) {

#ifdef _DEBUG
	ImGui::Begin("ステージエディター");
	ImGui::Text("%s", texName_[selectNumber_].c_str());
	ImGui::SliderInt("設置するマップの種類", &selectNumber_, 1, (int)(TD_10days::LevelMapChip::MapChip::kGoal));
	ImTextureID textureID = TextureManager::GetInstance()->GetTexture(TextureManager::Load(texPath_[selectNumber_]))->GetTextureID();
	ImGui::Image(textureID, ImVec2(128, 128));
	ImGui::Text("%s", "編集しているステージ");
	// 左矢印
	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		if (guiSelectNum_ > 1) {
			if (not isSave_) {
				if (NotSaveMoveConfirmation()) {
					guiSelectNum_--;
					LoadStage();
					isSave_ = true;
				}

			}
			else {
				guiSelectNum_--;
				LoadStage();
			}
		}
	}
	ImGui::SameLine();
	// 現在値を表示
	ImGui::Text("%d", guiSelectNum_);
	ImGui::SameLine();
	// 右矢印
	if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
		if (guiSelectNum_ < kStageMax_) {
			if (not isSave_) {
				if (NotSaveMoveConfirmation()) {
					guiSelectNum_++;
					LoadStage();
					isSave_ = true;
				}
			}
			else {
				guiSelectNum_++;
				LoadStage();
			}
		}

	}

	ImGui::Text("現在のマップ上限 縦幅 ＝ %d, 横幅 ＝ %d", levelMapChip_.GetSize().first, levelMapChip_.GetSize().second);

	ImGui::DragInt("横幅", &mapSize_.second, 1.0f, 0, 999);
	ImGui::DragInt("縦幅", &mapSize_.first, 1.0f, 0, 300);

	if (ImGui::Button("マップのサイズを上記に変更する")) {
		if (OperationConfirmation()) {
			levelMapChip_.Resize(mapSize_.first, mapSize_.second);
		}
	}
	//ImGui::DragFloat2("座標", mousePos.data(), 0.1f);
	//ImGui::DragInt("マップチップX", &tilePos_.first);
	//ImGui::DragInt("マップチップY", &tilePos_.second);
	if (ImGui::Button("現在のステージを保存する")) {
		if (OperationConfirmation()) {
			SaveFile(kFileName_);
			isSave_ = true;
		}
	}

	ImGui::End();
#endif // DEBUG_


}

void StageEditor::LoadFileAll() {

}

void StageEditor::ClickPushMove([[maybe_unused]] Vector2 mousePos) {

}

void StageEditor::SaveFile([[maybe_unused]] const std::string &fileName) {
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}
	// 書き込むcsvファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName + std::to_string(guiSelectNum_).c_str() + ".csv";
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込みように開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail()) {
		std::wstring message = L"Failed open data file for write.";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ファイルないよぉ！", 0);
		return;
	}

	for (int32_t y = 0; y < mapSize_.first; ++y) {
		for (int32_t x = 0; x < mapSize_.second; ++x) {
			ofs << static_cast<int>(levelMapChip_[y][x]); // enum → int
			if (x + 1 < mapSize_.second) {
				ofs << ",";
			}
		}
		ofs << "\n";
	}

	//ファイルを閉じる
	ofs.close();

	std::string message = "File save completed.";
	MessageBoxA(WinApp::GetInstance()->GetHWND(), message.c_str(), "Object", 0);
}

void StageEditor::ChackFiles() {
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::wstring message = L"Failed open data file for write.";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ディレクトリないよぉ！", 0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry &entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

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

void StageEditor::LoadFile(const std::string &fileName) {
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

TD_10days::LevelMapChip::MapChip StageEditor::NumberToMap(const int32_t num) {
	TD_10days::LevelMapChip::MapChip map{};

	if (num == 0) {
		map = TD_10days::LevelMapChip::MapChip::kEmpty;
	}
	if (num == 1) {
		map = TD_10days::LevelMapChip::MapChip::kWall;
	}
	if (num == 2) {
		map = TD_10days::LevelMapChip::MapChip::kTile;
	}
	if (num == 3) {
		map = TD_10days::LevelMapChip::MapChip::kFloor;
	}
	if (num == 4) {
		map = TD_10days::LevelMapChip::MapChip::kWater;
	}
	if (num == 5) {
		map = TD_10days::LevelMapChip::MapChip::kStart;
	}
	if (num == 6) {
		map = TD_10days::LevelMapChip::MapChip::kGoal;
	}

	return map;
}

int32_t StageEditor::MapToNumber(const TD_10days::LevelMapChip::MapChip map) {
	int32_t num{};

	if (map == TD_10days::LevelMapChip::MapChip::kEmpty) {
		num = 0;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kWall) {
		num = 1;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kTile) {
		num = 2;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kFloor) {
		num = 3;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kWater) {
		num = 4;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kStart) {
		num = 5;
	}
	if (map == TD_10days::LevelMapChip::MapChip::kGoal) {
		num = 6;
	}

	return num;
}

void StageEditor::LoadStage() {
	int32_t selectNum = guiSelectNum_;

	if (csvFile_.Load(kDirectoryPath_ + kFileName_ + std::to_string(selectNum).c_str() + ".csv")) {
		csvData_ = csvFile_;

		levelMapChip_.Init(csvData_);
	}
	else {
		levelMapChip_.Init(mapSize_.first, mapSize_.second);
	}

	ApplyMapChips();
}

bool StageEditor::LoadChackItem(const std::string &fileName) {
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

bool StageEditor::NotSaveMoveConfirmation() {
	int result = MessageBox(WinApp::GetInstance()->GetHWND(), L"保存をしていませんがこの操作を続けますか?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);
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

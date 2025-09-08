#include "StageEditor.h"
#include <SelectToGame/SelectToGame.h>
#include <iostream>


StageEditor::~StageEditor() {

}

void StageEditor::ApplyMapChips() {
	levelMapChip_.CreateHitBox();
	levelMapChip_.FindActionChips();
	
}

void StageEditor::InitOnce() {

	// もしテクスチャの名前がないなら
	if (texName_.empty()) {
		//ここに名前を入れる
		texName_ = {
			{},
			{("wall")},//wall
			{("tile")},//tile
			{("floor")},//floor
			{("water")},//water
			{("start")},//start
			{("goal")},//goal
			{("needle")},//needle
			{("mesh")},//mesh
		};
	}
	// もしテクスチャのパスがないなら
	if (texPath_.empty()) {
		texPath_ = {
			{},
			{("StageTex/block.png")},//wall
			{("StageTex/floor2.png")},//tile
			{("StageTex/floor.png")},//floor
			{("StageTex/blue.png")},//water
			{("StageTex/red.png")},//start
			{("StageTex/goal.png")},//goal
			{("StageTex/Needle.png")},//needle
			{("StageTex/Net.png")},//mesh
		};
	}

	// もし仮にテクスチャパスと個数が合わなかったら
	if (levelMapChip_.GetMapChipData().size() != texPath_.size()) {
		// テクスチャなどの情報を与える
		levelMapChip_.SetMapChipData(
			{
			{false},	// Air
			{TextureHandle{TextureManager::Load(texPath_[1])}},//wall
			{TextureHandle{TextureManager::Load(texPath_[2])}},//tile
			{TextureHandle{TextureManager::Load(texPath_[3])}},//floor
			{TextureHandle{TextureManager::Load(texPath_[4])}},//water
			{TextureHandle{TextureManager::Load(texPath_[5])}, false},//start
			{TextureHandle{TextureManager::Load(texPath_[6])}, false},//goal
			{TextureHandle{TextureManager::Load(texPath_[7])}, false, true},//needle
			{TextureHandle{TextureManager::Load(texPath_[8])}, true, false},//Mesh
			});
	}

	if (not newTex_) {
		newTex_ = std::make_unique<Tex2DState>();
		newTex_->transform.translate_ = Vector2(0, 0);
		newTex_->transform.scale_ = { 1.f,1.f };
		newTex_->color = 0x0000ffff;
		//配置用のものなのでwhiteTex
		newTex_->sprite = Sprite::Generate();
	}

}

void StageEditor::Initialize(TD_10days::LevelMapChipRenderer *pLevelMapChipRender) {
	// 描画処理の型を渡す｡(本来はオブザーバーパターンでやるべき)
	pLevelMapChipRender_ = pLevelMapChipRender;

	// 一度しか呼び出さない初期化処理(一度初期化されていた場合､内部のifで全て流される)
	InitOnce();

	//死んだときに更新しないように
	if (stageNum_ != SelectToGame::GetInstance()->GetStageNum()){
		// ステージ番号の取得
		stageNum_ = SelectToGame::GetInstance()->GetStageNum();

		// ステージ番号の基数を1にする
		int32_t selectNum = stageNum_ + 1;

		guiSelectNum_ = selectNum;

		// ステージの読み込みを行う
		if (csvFile_.Load(kDirectoryPath_ + kFileName_ + std::to_string(selectNum).c_str() + ".csv")) {
			// CSVが読み込めたらそれに応じた初期化を行う｡
			csvData_ = csvFile_;

			levelMapChip_.Init(csvData_);
		}
		else {
			// ステージが読み込めなかった場合､とりあえず初期設定で作る
			levelMapChip_.Init(mapSize_.first, mapSize_.second);

			// 仮配置
			std::fill(levelMapChip_[0].begin(), levelMapChip_[0].end(), TD_10days::LevelMapChip::MapChip::kFloor);
			levelMapChip_[1][0] = TD_10days::LevelMapChip::MapChip::kWall;
			levelMapChip_[1][2] = TD_10days::LevelMapChip::MapChip::kWall;
			levelMapChip_[2][0] = TD_10days::LevelMapChip::MapChip::kWall;
			levelMapChip_[3][0] = TD_10days::LevelMapChip::MapChip::kWall;
		}
	}

	
}

void StageEditor::Finalize() {

}

void StageEditor::Update() {

#ifdef _DEBUG
	if (isUseEditor_) {

		if (not ImGui::GetIO().WantCaptureMouse) {
			//マウスの座標をアプリと合わせる
			Vector2 mousePos = Vector2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
			mousePos.y *= -1; // Y反転（上が正になる）
			mousePos += Vector2{ -640, 360 };
			//カメラの距離に合わせてサイズを変更
			blockSize_ = (int)(1.0f / camera_.scale_);

			//カメラを考慮した座標に変換
		
			world.first = (int32_t)(mousePos.x + (camera_.translation_.x * blockSize_));
			world.second = (int32_t)(mousePos.y + (camera_.translation_.y * blockSize_));

			int blockQuater = (blockSize_ / 4);
			// ---- マップチップの範囲内かどうか ----
			isIncide_ =
				((-(blockQuater * 2) <= world.first) and (world.first < (mapSize_.second * blockSize_) - (blockQuater * 3))) and
				((-(blockQuater * 2) <= world.second) and (world.second < (mapSize_.first * blockSize_) - (blockQuater * 3)));

			if (isIncide_) {
				// ---- どのマップチップか ----
				tilePos_.first = (world.first + (blockSize_ / 2)) / blockSize_;
				tilePos_.second = (world.second + (blockSize_ / 2)) / blockSize_;

				newTex_->transform.translate_ = { (float)(tilePos_.first),(float)(tilePos_.second) };

				//左クリックしたら
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
					if (levelMapChip_[tilePos_.second][tilePos_.first] != NumberToMap(selectNumber_)) {
						levelMapChip_[tilePos_.second][tilePos_.first] = NumberToMap(selectNumber_);
						pLevelMapChipRender_->CalcSpriteData();
						isSave_ = false;
					}
				}
				//右クリックしたら
				else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
					if (levelMapChip_[tilePos_.second][tilePos_.first] != NumberToMap(0)) {
						levelMapChip_[tilePos_.second][tilePos_.first] = NumberToMap(0);
						pLevelMapChipRender_->CalcSpriteData();
						isSave_ = false;
					}
				}

			}
			//離した時に当たり判定の更新
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) or ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				ApplyMapChips();
			}
		}
	}

	Debug(Vector2({ (float)(world.first),(float)(world.second) }));

#endif // _DEBUG

}

void StageEditor::PutDraw() {

#ifdef _DEBUG

	if (isIncide_ or isUseEditor_) {
		newTex_->sprite->SetColor(newTex_->color);
		newTex_->sprite->SetPosition(newTex_->transform.translate_);
		newTex_->sprite->SetScale(newTex_->transform.scale_);
		newTex_->sprite->SetRotate(newTex_->transform.rotate_.Get());
		newTex_->sprite->SetPivot(kPivotValue_);
		newTex_->sprite->SetInvertY(true);
		newTex_->sprite->Draw();
	}

#endif // _DEBUG

}

void StageEditor::Debug([[maybe_unused]] Vector2 mousePos) {

#ifdef _DEBUG
	ImGui::Begin("ステージエディター");
	ImGui::Text("%s", texName_[selectNumber_].c_str());
	ImGui::SliderInt("設置するマップの種類", &selectNumber_, 1, static_cast<int>(TD_10days::LevelMapChip::MapChip::CountElements) - 1u);
	const ImTextureID textureID = TextureManager::GetInstance()->GetTexture(TextureManager::Load(texPath_[selectNumber_]))->GetTextureID();
	ImGui::Image(textureID, ImVec2(128, 128));
	ImGui::Text("%s", "編集しているステージ");
	// 左矢印
	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		if (guiSelectNum_ > 1) {
			if (not isSave_) {
				if (OperationConfirmation(L"保存をしていませんがこの操作を続けますか?")) {
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
				if (OperationConfirmation(L"保存をしていませんがこの操作を続けますか?")) {
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

	ImGui::DragInt("縦幅", &mapSize_.first, 1.0f, 0, 300);
	ImGui::DragInt("横幅", &mapSize_.second, 1.0f, 0, 999);

	if (mapSize_.first != nowMapSize_.first or mapSize_.second != nowMapSize_.second){
		//サイズ変更してないけど上記の変更したいバーが変わっている状態
		isNotChangeRange_ = true;
	}

	if (ImGui::Button("マップのサイズを上記に変更する")) {
		if (OperationConfirmation(L"このステージのサイズを変更しますか?")) {
			levelMapChip_.Resize(mapSize_.first, mapSize_.second);
			nowMapSize_ = mapSize_;
		}
	}
	if (ImGui::Button("現在のステージを保存する")) {
		if (OperationConfirmation(L"このステージを保存しますか?")) {
			if (isNotChangeRange_){
				if (OperationConfirmation(L"サイズの変更をしていませんが変更しますか？")) {
					levelMapChip_.Resize(mapSize_.first, mapSize_.second);
					nowMapSize_ = mapSize_;
				}
				SaveFile(kFileName_);
				isSave_ = true;
			}
			else {
				SaveFile(kFileName_);
				isSave_ = true;
			}			
		}
	}
	SwapStage();

	ImGui::End();

	ImGui::Begin("Stageエディター使用状況");
	ImGui::Checkbox("Stageエディターを利用しているか", &isUseEditor_);
	ImGui::End();
#endif // DEBUG_


}

void StageEditor::SaveFile([[maybe_unused]] const std::string &fileName) {
	std::filesystem::path dir(kDirectoryPath_);
	if (not std::filesystem::exists(dir)) {
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

	for (int32_t y = 0; y < nowMapSize_.first; ++y) {
		for (int32_t x = 0; x < nowMapSize_.second; ++x) {
			ofs << static_cast<int>(levelMapChip_[y][x]); // enum → int
			if (x + 1 < nowMapSize_.second) {
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

TD_10days::LevelMapChip::MapChip StageEditor::NumberToMap(const int32_t num) {
	TD_10days::LevelMapChip::MapChip map{ static_cast<TD_10days::LevelMapChip::MapChip>(num) };

	return map;
}

int32_t StageEditor::MapToNumber(const TD_10days::LevelMapChip::MapChip map) {
	int32_t num{ static_cast<int32_t>(map) };

	return num;
}

void StageEditor::LoadStage() {
	int32_t selectNum = guiSelectNum_;
	SelectToGame::GetInstance()->SetStageNum(selectNum - 1);

	if (csvFile_.Load(kDirectoryPath_ + kFileName_ + std::to_string(selectNum).c_str() + ".csv")) {
		csvData_ = csvFile_;

		levelMapChip_.Init(csvData_);
	}
	else {
		levelMapChip_.Init(nowMapSize_.first, nowMapSize_.second);
	}
	pLevelMapChipRender_->CalcSpriteData();
	ApplyMapChips();
}

void StageEditor::SwapStage(){
	static int stageA = 0; // 選択中のステージA
	static int stageB = 1; // 選択中のステージB

	std::vector<std::string> stages;
	for (auto& entry : std::filesystem::directory_iterator("resources/Datas/StageData")) {
		if (entry.path().extension() == ".csv") {
			stages.push_back(entry.path().string());
		}
	}

	// コンボでステージ選択
	ImGui::Text("入れ替えたいステージを選択してください");

	ImGui::Combo("Stage A", &stageA,
		[](void* data, int idx, const char** out_text) {
			auto* vec = (std::vector<std::string>*)data;
			if (idx < 0 || idx >= (int)vec->size()) return false;
			static std::string display;
			// ファイル名（例: stage1.csv）
			std::string filename = std::filesystem::path((*vec)[idx]).stem().string();

			// "stage" の後ろの数字を取り出して "StageDataN" に変換
			if (filename.rfind("stage", 0) == 0) { // "stage" で始まるか確認
				std::string number = filename.substr(5); // "stage" の5文字を飛ばす
				display = "StageData" + number;
			}
			else {
				display = filename; // 想定外の名前ならそのまま表示
			}

			*out_text = display.c_str();
			return true;
		},
		(void*)&stages, (int)stages.size());

	ImGui::Combo("Stage B", &stageB,
		[](void* data, int idx, const char** out_text) {
			auto* vec = (std::vector<std::string>*)data;
			if (idx < 0 || idx >= (int)vec->size()) return false;
			static std::string display;
			// ファイル名
			std::string filename = std::filesystem::path((*vec)[idx]).stem().string();

			// "stage" の後ろの数字を取り出して "StageDataN" に変換
			if (filename.rfind("stage", 0) == 0) { // "stage" で始まるか確認
				std::string number = filename.substr(5); // "stage" の5文字を飛ばす
				display = "StageData" + number;
			}
			else {
				display = filename; // 想定外の名前ならそのまま表示
			}

			*out_text = display.c_str();
			return true;
		},
		(void*)&stages, (int)stages.size());

	// ボタンを押したら入れ替え
	if (ImGui::Button("ステージを入れ替え")) {
		if (stageA != stageB) {
			try {
				std::string fileA = stages[stageA];
				std::string fileB = stages[stageB];

				std::string tmp = "stage_tmp.csv";

				std::filesystem::rename(fileA, tmp);
				std::filesystem::rename(fileB, fileA);
				std::filesystem::rename(tmp, fileB);

				std::wstring message = L"入れ替えが完了しました";
				MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"Success!", 0);

				LoadStage();

			}
			catch (const std::filesystem::filesystem_error& e) {
				std::cerr << "エラー: " << e.what() << std::endl;
			}
		}
		else {
			std::wstring message = L"同じファイルが選択されています";
			MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"Faile", 0);
		}
	}
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

bool StageEditor::OperationConfirmation(const std::wstring text){
	int result = MessageBox(WinApp::GetInstance()->GetHWND(), text.c_str(), L"Confirmation", MB_YESNO | MB_ICONQUESTION);
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

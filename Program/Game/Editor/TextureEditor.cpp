#include "TextureEditor.h"
#include <fstream>

TextureEditor::~TextureEditor(){

}

void TextureEditor::Initialize() {
	sceneName_[SceneID::Title] = "Title";
	sceneName_[SceneID::Game] = "Game";
	sceneName_[SceneID::StageSelect] = "Select";
	sceneName_[SceneID::Result] = "Result";

	newTex_ = std::make_unique<Tex2DState>();
	newTex_->transform.translate_ = Vector2(0, 0);
	newTex_->transform.scale_ = { 32,32 };
	newTex_->color = 0xff0000ff;
	//配置用のものなのでwhiteTex
	newTex_->sprite = Sprite::Generate(TextureManager::LoadDefaultTexture());

	camera_.Identity();
	//最初に読み込んでおく
	auto file = GetFilePathFormDir("./resources/UI/", ".png");
	for (auto& i : file) {
		TextureManager::Load(i.string().c_str());
	}
	//最初なのでタイトルに設定
	SetSceneId(SceneID::Title);

	//LoadFileAll();
}

void TextureEditor::Finalize() {

}

void TextureEditor::Update() {

#ifdef _DEBUG	
	
	//マウスの座標をアプリと合わせる
	Vector2 mousePos = Vector2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	if (isDecideToPlace_) {
		//左クリックしたら
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			//マウスの位置に合わせる
			newTex_->transform.translate_ = mousePos;
		}
	}
	Debug(id_, mousePos);
	
	ClickPushMove(id_, mousePos);

	//マウスが画像と重なっているか
	for (size_t i = 0; i < static_cast<size_t>(SceneID::kNum); i++) {
		if (i != static_cast<size_t>(id_))
			continue;
		for (size_t j = 0; j < texies_[i].size(); j++) {
			uint32_t beforeColor = texColors_[i][j];
			if (UICollider::IsMouseOverRotatedRect(texies_[i][j]->transform.translate_, texies_[i][j]->transform.rotate_.Get(), texies_[i][j]->transform.scale_)) {
				//重複がしないように
				if (selectNumber_ == -1 || selectNumber_ == int(j)) {
					SoLib::Color::RGB4 color4 = texies_[i][j]->color;
					color4.a = 1.0f - 0.5f;
					
					texies_[i][j]->color = color4;
				}
				else {
					texies_[i][j]->color = beforeColor;
				}

			}
			else {
				texies_[i][j]->color = beforeColor;
			}
		}
	}


	//ShortCutMove(id_);
#endif // _DEBUG

}

void TextureEditor::Draw() {

	for (size_t i = 0; i < static_cast<size_t>(SceneID::kNum); i++) {
		if (i != static_cast<size_t>(id_))
			continue;
		for (size_t j = 0; j < texies_[i].size(); j++) {
			texies_[i][j]->sprite->SetColor(texies_[i][j]->color);
			texies_[i][j]->sprite->SetPosition(texies_[i][j]->transform.translate_);
			texies_[i][j]->sprite->SetScale(texies_[i][j]->transform.scale_);
			texies_[i][j]->sprite->SetRotate(texies_[i][j]->transform.rotate_.Get());
			texies_[i][j]->sprite->SetTexOrigin(texies_[i][j]->uvTransform.translate_);
			texies_[i][j]->sprite->SetTexDiff(texies_[i][j]->uvTransform.scale_);

			texies_[i][j]->sprite->SetPivot(kPivotValue_);

			texies_[i][j]->sprite->Draw();
		}
	}
}

void TextureEditor::PutDraw() {
	if (isDecideToPlace_) {
		newTex_->sprite->SetColor(newTex_->color);
		newTex_->sprite->SetPosition(newTex_->transform.translate_);
		newTex_->sprite->SetScale(newTex_->transform.scale_);
		newTex_->sprite->SetRotate(newTex_->transform.rotate_.Get());
		newTex_->sprite->SetPivot(kPivotValue_);

		newTex_->sprite->Draw();
	}
}

void TextureEditor::Debug([[maybe_unused]] const SceneID id, [[maybe_unused]] Vector2 mousePos) {
#ifdef _DEBUG	
	// 例: ユーザーがUIのテクスチャを選んだときなど
	bool shouldFocus = false;
	if (selectNumber_ != -1) {
		shouldFocus = true;
	}
	if (isTextureEditor_) {
		ImGui::Begin("UIエディター", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar()) {
			if (isDecideToPlace_) {
				if (ImGui::BeginMenu("UI生成")) {
					ImGui::DragFloat2("生成するポジション", &newTex_->transform.translate_.x, 1.0f);
					ImGui::DragFloat2("生成する大きさ", &newTex_->transform.scale_.x, 1.0f);
					ImGui::DragInt("生成する角度", &newTex_->angle_degrees, 1, -360, 360);
					/*一旦ブレンドの部分は無視*/
					//// コンボボックスを使ってenumの値を選択する
					//if (ImGui::BeginCombo("BlendType", kBlendTypeStrs[static_cast<uint32_t>(baseBlend_)].c_str())) {
					//	for (uint32_t i = 0; i < static_cast<uint32_t>(BlendType::kNum); ++i) {
					//		bool isSelected = (baseBlend_ == static_cast<BlendType>(i));
					//		if (ImGui::Selectable(kBlendTypeStrs[i].c_str(), isSelected)) {
					//			baseBlend_ = static_cast<BlendType>(i);
					//		}
					//		if (isSelected) {
					//			ImGui::SetItemDefaultFocus();
					//		}
					//	}
					//	ImGui::EndCombo();
					//}
					if (ImGui::TreeNode("生成するUI画像")) {
						// 子ウィンドウ（スクロール可能領域）を作成
						ImGui::BeginChild("TextureListScroll", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

						// レイアウトのための列数（1列なら省略してもよい）
						int columnCount = 2;
						int index = 0;
						auto file = GetFilePathFormDir("./resources/UI/", ".png");
						for (auto& i : file) {
							// reinterpret_cast ではなくサイズを明確に扱える union や uintptr_t を使う
							ImTextureID textureID = TextureManager::GetInstance()->GetTexture(TextureManager::Load(i.string().c_str()))->GetTextureID();
							ImGui::PushID(index);

							if (ImGui::ImageButton(i.string().c_str(), textureID, ImVec2(128, 128))) {
								if (OperationConfirmation()) {
									NewTexMake(id, i.string().c_str());
								}
								ImGui::PopID();
								break;
							}

							// マウスがこのImageButtonの上にあるときにツールチップ表示
							if (ImGui::IsItemHovered()) {
								ImGui::BeginTooltip();
								ImGui::TextUnformatted(i.string().c_str()); // ファイルのフルパス表示
								ImGui::EndTooltip();
							}

							DragMove(i);

							DropMove(id_, mousePos);
							ImGui::PopID();
							// グリッド状に配置（改行）
							if ((index + 1) % columnCount != 0) {
								ImGui::SameLine();
							}
							++index;
						}
						ImGui::EndChild();
						ImGui::TreePop();
					}
					ImGui::EndMenu();
				}
			}
			if (ImGui::BeginMenu("ファイル関連")) {
				for (auto& i : sceneName_) {
					if (i.first != id) {
						continue;
					}
					if (ImGui::Button(("現在のシーンを保存 " + i.second).c_str())) {
						stageName_ = i.second.c_str();
						if (OperationConfirmation()) {
							SaveFile(i.second.c_str());
						}
						break;
					}
				}
				if (ImGui::Button("全てのシーンを保存する")) {
					if (OperationConfirmation()) {
						for (auto& i : sceneName_) {
							stageName_ = i.second.c_str();
							SaveFile(i.second.c_str());
						}
					}
				}

				if (ImGui::TreeNode("ファイル読み込み")) {
					auto file = GetFilePathFormDirOrigin(kDirectoryPath_, ".json");

					for (auto& i : file) {
						if (ImGui::Button(i.string().c_str())) {
							if (OperationConfirmation()) {
								LoadFiles(i.string());
							}
							break;
						}
					}
					if (ImGui::Button("全てのシーンのUIを読み込む")) {
						if (OperationConfirmation()) {
							LoadFileAll();
						}
					}
					ImGui::TreePop();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("エディター機能")) {
				ImGui::Checkbox("新規画像配置機能", &isDecideToPlace_);
				ImGui::Checkbox("選択移動機能", &isSelecteTex_);
				ImGui::Checkbox("ショートカットキー機能", &isShortCuts_);
				ImGui::Checkbox("サイズを元の物にする機能", &isUseTextureBaseSize_);
				// 先に状態を保存
				bool wasPush = isPush_;
				bool wasTrigger = isTriger_;

				// チェックボックス描画
				/*ImGui::Checkbox("長押しで選択", &isPush_);
				ImGui::SameLine();
				ImGui::Checkbox("クリックで選択", &isTriger_);*/

				if (isPush_ && !wasPush) {
					isTriger_ = false;
				}
				if (isTriger_ && !wasTrigger) {
					isPush_ = false;
				}


				ImGui::EndMenu();
			}
			/*if (ImGui::BeginMenu("ショートカットの説明")) {
				ImGui::Text("機能にてチェックボックスにマークを入れると利用可能になります");
				ImGui::Text("左ctrl + s		現在のシーンを保存");
				ImGui::Text("左ctrl + l		現在のシーンのファイルを読み込む");
				ImGui::Text("画像を選択し、backspace OR DELETE		画像の削除");
				ImGui::Text("削除に関してはaキーを追加で押しているとシーン内の画像すべてを削除できます");

				ImGui::EndMenu();
			}*/
			ImGui::EndMenuBar();

		}

		ImGui::End();

		if (shouldFocus) {
			ImGui::SetNextWindowFocus();
			shouldFocus = false;
		}

		ImGui::Begin("配置されているUI");
		auto& texList = texies_[static_cast<size_t>(id)];
		auto& texColorList = texColors_[static_cast<size_t>(id)];
		for (size_t i = 0; i < texList.size(); i++) {
			std::string lebel = texList[i]->textureName.c_str() + std::to_string(i);

			if (static_cast<int>(i) == selectTexImguiNumber_) {
				// 選択されているツリー項目を表示位置の上寄りにスクロール
				ImGui::SetScrollHereY(0.1f); // 上に近い位置 (0.0f で最上部)

				ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			}
			else {
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);
			}

			if (ImGui::TreeNode(lebel.c_str())) {
				// ユーザーがこのツリーを開いた → 選択扱いにする（必要に応じて）
				if (selectTexImguiNumber_ != static_cast<int>(i)) {
					selectTexImguiNumber_ = static_cast<int>(i);
				}
				ImGui::DragFloat2("ポジション", &texList[i]->originalTransform.translate_.x, 1.0f);
				ImGui::DragFloat2("大きさ", &texList[i]->originalTransform.scale_.x, 1.0f);
				ImGui::DragInt("角度", &texList[i]->angle_degrees, 1, -360, 360);
				texList[i]->originalTransform.rotate_ = DegreeToRadian(texList[i]->angle_degrees);
				texList[i]->transform = texList[i]->originalTransform;
				ImGui::DragFloat2("UVポジション", &texList[i]->uvTransform.translate_.x, 0.01f);
				ImGui::DragFloat2("UV大きさ", &texList[i]->uvTransform.scale_.x, 0.01f);
				SoLib::Color::RGB4 color = texColorList[i];
				ImGui::ColorEdit4("テクスチャの色", color.data(), true);
				texList[i]->color = color;
				texColorList[i] = color;
				/*if (ImGui::BeginCombo("BlendType", kBlendTypeStrs[static_cast<uint32_t>(texList[i]->blend)].c_str())) {
					for (uint32_t j = 0; j < static_cast<uint32_t>(BlendType::kNum); ++j) {
						bool isSelected = (texList[i]->blend == static_cast<BlendType>(j));
						if (ImGui::Selectable(kBlendTypeStrs[j].c_str(), isSelected)) {
							texList[i]->blend = static_cast<BlendType>(j);
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}*/
				if (ImGui::Button("このテクスチャを削除する")) {
					if (OperationConfirmation()) {
						texList.erase(texList.begin() + i);
						texColorList.erase(texColorList.begin() + i);
						i--;
					}
				}

				if (ImGui::TreeNode("変更するUI画像")) {
					// 子ウィンドウ（スクロール可能領域）を作成
					ImGui::BeginChild("TextureListScroll", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

					// レイアウトのための列数（1列なら省略してもよい）
					int columnCount = 2;
					int index = 0;
					std::string filePath = "./resources/UI/";
					std::string sceneFileName = "";
					if (id == SceneID::Title) {
						sceneFileName = "Title\\";
					}
					else if (id == SceneID::StageSelect) {
						sceneFileName = "Select\\";
					}
					else if (id == SceneID::Game) {
						sceneFileName = "InGame\\";
					}
					else if (id == SceneID::Result) {
						sceneFileName = "Result\\";
					}
					filePath += sceneFileName;

					auto file = GetFilePathFormDir(filePath.c_str(), ".png");
					for (auto& t : file) {
						// reinterpret_cast ではなくサイズを明確に扱える union や uintptr_t を使う
						ImTextureID textureID = TextureManager::GetInstance()->GetTexture(TextureManager::Load(t.string().c_str()))->GetTextureID();

						if (ImGui::ImageButton(t.string().c_str(), textureID, ImVec2(128, 128))) {
							if (OperationConfirmation()) {
								ChangeTex(t.string().c_str(), texList[i].get());
							}
							break;
						}

						// グリッド状に配置（改行）
						if ((index + 1) % columnCount != 0) {
							ImGui::SameLine();
						}
						++index;
					}
					ImGui::EndChild();
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
	ImGui::Begin("UIエディター使用状況");
	ImGui::Checkbox("エディターを利用しているか", &isTextureEditor_);
	ImGui::End();

#endif // _DEBUG
}

void TextureEditor::LoadFiles(const std::string& fileName) {
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::wstring message = L"This file path does not exist.";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ディレクトリないよぉ！", 0);;
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

		if (filePath == fileName) {
			//ファイル読み込み
			LoadFile(fileName);
			return;
		}
	}
	std::wstring message = L"ファイルが見つかりませんでした。";
	MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"UIないよぉ！", 0);
}

void TextureEditor::SaveFile(const std::string& fileName) {
	//保存
	json root;
	root = json::object();
	root[kItemName_] = json::object();
	root[kItemName_][fileName];
	std::string sceneTag;
	/*
	* 座標
	* 大きさ
	* 色
	* テクスチャの名前
	* テクスチャのフルパス
	*
	*/

	/*保存するものを書き込む*/
	if (sceneName_[SceneID::Title] == fileName) {
		sceneTag = "Title";
		for (size_t i = 0; i < texies_[0].size(); i++) {
			root[kItemName_][sceneTag.c_str()][i]["translate"] = json::array({
				   texies_[0][i]->originalTransform.translate_.x,
				   texies_[0][i]->originalTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["scale"] = json::array({
				   texies_[0][i]->originalTransform.scale_.x,
				   texies_[0][i]->originalTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVTranslate"] = json::array({
				   texies_[0][i]->uvTransform.translate_.x,
				   texies_[0][i]->uvTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVScale"] = json::array({
				   texies_[0][i]->uvTransform.scale_.x,
				   texies_[0][i]->uvTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["degree"] = texies_[0][i]->angle_degrees;
			root[kItemName_][sceneTag.c_str()][i]["color"] = texies_[0][i]->color;
			root[kItemName_][sceneTag.c_str()][i]["TextureName"] = texies_[0][i]->textureName;
			root[kItemName_][sceneTag.c_str()][i]["TextureFullPath"] = texies_[0][i]->textureFullPath;
		}

	}
	else if (sceneName_[SceneID::Game] == fileName) {
		sceneTag = "Game";
		for (size_t i = 0; i < texies_[1].size(); i++) {
			root[kItemName_][sceneTag.c_str()][i]["translate"] = json::array({
				   texies_[1][i]->originalTransform.translate_.x,
				   texies_[1][i]->originalTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["scale"] = json::array({
				   texies_[1][i]->originalTransform.scale_.x,
				   texies_[1][i]->originalTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVTranslate"] = json::array({
				   texies_[1][i]->uvTransform.translate_.x,
				   texies_[1][i]->uvTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVScale"] = json::array({
				   texies_[1][i]->uvTransform.scale_.x,
				   texies_[1][i]->uvTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["degree"] = texies_[1][i]->angle_degrees;
			root[kItemName_][sceneTag.c_str()][i]["color"] = texies_[1][i]->color;
			root[kItemName_][sceneTag.c_str()][i]["TextureName"] = texies_[1][i]->textureName;
			root[kItemName_][sceneTag.c_str()][i]["TextureFullPath"] = texies_[1][i]->textureFullPath;
		}

	}
	else if (sceneName_[SceneID::StageSelect] == fileName) {
		sceneTag = "Select";
		for (size_t i = 0; i < texies_[2].size(); i++) {
			root[kItemName_][sceneTag.c_str()][i]["translate"] = json::array({
				   texies_[2][i]->originalTransform.translate_.x,
				   texies_[2][i]->originalTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["scale"] = json::array({
				   texies_[2][i]->originalTransform.scale_.x,
				   texies_[2][i]->originalTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVTranslate"] = json::array({
				   texies_[2][i]->uvTransform.translate_.x,
				   texies_[2][i]->uvTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVScale"] = json::array({
				   texies_[2][i]->uvTransform.scale_.x,
				   texies_[2][i]->uvTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["degree"] = texies_[2][i]->angle_degrees;
			root[kItemName_][sceneTag.c_str()][i]["color"] = texies_[2][i]->color;
			root[kItemName_][sceneTag.c_str()][i]["TextureName"] = texies_[2][i]->textureName;
			root[kItemName_][sceneTag.c_str()][i]["TextureFullPath"] = texies_[2][i]->textureFullPath;
		}

	}
	else if (sceneName_[SceneID::Result] == fileName) {
		sceneTag = "Result";
		for (size_t i = 0; i < texies_[3].size(); i++) {
			root[kItemName_][sceneTag.c_str()][i]["translate"] = json::array({
				   texies_[3][i]->originalTransform.translate_.x,
				   texies_[3][i]->originalTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["scale"] = json::array({
				   texies_[3][i]->originalTransform.scale_.x,
				   texies_[3][i]->originalTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVTranslate"] = json::array({
				   texies_[3][i]->uvTransform.translate_.x,
				   texies_[3][i]->uvTransform.translate_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["UVScale"] = json::array({
				   texies_[3][i]->uvTransform.scale_.x,
				   texies_[3][i]->uvTransform.scale_.y
				});
			root[kItemName_][sceneTag.c_str()][i]["degree"] = texies_[3][i]->angle_degrees;
			root[kItemName_][sceneTag.c_str()][i]["color"] = texies_[3][i]->color;
			root[kItemName_][sceneTag.c_str()][i]["TextureName"] = texies_[3][i]->textureName;
			root[kItemName_][sceneTag.c_str()][i]["TextureFullPath"] = texies_[3][i]->textureFullPath;

		}

	}
	else {
		std::wstring message = L"この名前のシーンが見つかりませんでした。";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"シーンないよぉ！", 0);
		return;
	}

	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::filesystem::create_directory(kDirectoryName_);
	}
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName + ".json";
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
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	std::string message = "File save completed.";
	MessageBoxA(WinApp::GetInstance()->GetHWND(), message.c_str(), "Object", 0);
}

void TextureEditor::ChackFiles() {
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

void TextureEditor::LoadFile(const std::string& fileName) {
	if (!LoadChackItem(fileName))
		return;
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = fileName;
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::wstring message = L"Failed open data file for write.";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ファイルないよぉ！", 0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	nlohmann::json::iterator itGroupUI = root.find(kItemName_);
	//未登録チェック
	assert(itGroupUI != root.end());


	//グループを検索
	nlohmann::json::iterator itGroupNum = root[kItemName_].find(sceneName_[id_].c_str());
	//未登録チェック
	if (itGroupNum == root[kItemName_].end()) {
		std::wstring message = L"選択したシーンと一致しませんでした。";
		MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"シーンが合ってないよぉ！", 0);
		return;
	}

	texies_[static_cast<size_t>(id_)].clear();
	texColors_[static_cast<size_t>(id_)].clear();

	for (const auto& i : root[kItemName_][sceneName_[id_].c_str()]) {
		std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
		Vector2 pos, scale;
		Vector2 UVpos, UVscale;
		from_json(i["scale"], scale);
		from_json(i["translate"], pos);
		from_json(i["UVScale"], UVscale);
		from_json(i["UVTranslate"], UVpos);
		setTex_->color = i["color"];
		uint32_t color = i["color"];
		setTex_->angle_degrees = i.value("degree", 0);
		setTex_->transform.rotate_ = DegreeToRadian(setTex_->angle_degrees);
		setTex_->transform.scale_ = scale;
		setTex_->transform.translate_ = pos;
		setTex_->originalTransform = setTex_->transform;
		setTex_->uvTransform.scale_ = UVscale;
		setTex_->uvTransform.translate_ = UVpos;
		TextureManager::Load(i["TextureFullPath"]);
		setTex_->sprite = Sprite::Generate(TextureManager::Load(i["TextureFullPath"]));
		setTex_->textureFullPath = i["TextureFullPath"];
		setTex_->textureName = i["TextureName"];


		texies_[static_cast<size_t>(id_)].emplace_back(std::move(setTex_));
		texColors_[static_cast<size_t>(id_)].emplace_back(std::move(color));
	}

#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WinApp::GetInstance()->GetHWND(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

void TextureEditor::LoadFileAll() {
	for (auto& scene : sceneName_) {
		std::string file = (kDirectoryPath_ + scene.second + ".json");
		if (!LoadChackItem(file))
			continue;
		//読み込むjsonファイルのフルパスを合成する
		std::string filePath = file;
		//読み込み用のファイルストリーム
		std::ifstream ifs;
		//ファイルを読み込み用に開く
		ifs.open(filePath);
		// ファイルオープン失敗
		if (ifs.fail()) {
			std::wstring message = L"Failed open data file for write.";
			MessageBoxW(WinApp::GetInstance()->GetHWND(), message.c_str(), L"ファイルないよぉ！", 0);
		}

		nlohmann::json root;

		//json文字列からjsonのデータ構造に展開
		ifs >> root;
		//ファイルを閉じる
		ifs.close();

		//グループを検索
		nlohmann::json::iterator itGroupUI = root.find(kItemName_);
		//未登録チェック
		assert(itGroupUI != root.end());

		texies_[static_cast<size_t>(scene.first)].clear();
		texColors_[static_cast<size_t>(scene.first)].clear();

		for (const auto& i : root[kItemName_][scene.second.c_str()]) {
			std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
			Vector2 pos, scale;
			Vector2 UVpos, UVscale;
			from_json(i["scale"], scale);
			from_json(i["translate"], pos);
			from_json(i["UVScale"], UVscale);
			from_json(i["UVTranslate"], UVpos);
			setTex_->color = i["color"];
			uint32_t color = i["color"];
			//setTex_->blend = i["blend"];
			setTex_->angle_degrees = i.value("degree", 0);
			setTex_->transform.rotate_ = DegreeToRadian(setTex_->angle_degrees);
			setTex_->transform.scale_ = scale;
			setTex_->transform.translate_ = pos;
			setTex_->originalTransform = setTex_->transform;
			setTex_->uvTransform.scale_ = UVscale;
			setTex_->uvTransform.translate_ = UVpos;
			TextureManager::GetInstance()->Load(i["TextureFullPath"]);
			setTex_->sprite = Sprite::Generate(TextureManager::Load(i["TextureFullPath"]));
			setTex_->textureFullPath = i["TextureFullPath"];
			setTex_->textureName = i["TextureName"];

			texies_[static_cast<size_t>(scene.first)].emplace_back(std::move(setTex_));
			texColors_[static_cast<size_t>(scene.first)].emplace_back(std::move(color));
		}


	}
}

void TextureEditor::ClickPushMove(const SceneID id, Vector2 mousePos) {

	//マウス左を押している間マウスについていくようにする
	for (size_t i = 0; i < static_cast<size_t>(SceneID::kNum); i++) {
		if (i != static_cast<size_t>(id))
			continue;
		for (size_t j = 0; j < texies_[i].size(); j++) {
			if (isSelecteTex_) {
				//左を押している間拾う
				if (ImGui::IsMouseDown(0) and selectNumber_ != -1) {
					texies_[i][selectNumber_]->originalTransform.translate_ = mousePos;
				}
			}
			if (UICollider::IsMouseOverRotatedRect(texies_[i][j]->transform.translate_, texies_[i][j]->transform.rotate_.Get(), texies_[i][j]->transform.scale_)) {

				if (ImGui::IsMouseDown(0)) {
					//重複がしないように
					if (selectNumber_ == -1 and !ImGui::GetIO().WantCaptureMouse) {
						selectNumber_ = int(j);
						selectTexNumber_ = selectNumber_;
						selectTexImguiNumber_ = selectNumber_;
					}
				}
				else {
					if (!ImGui::GetIO().WantCaptureMouse){
						selectNumber_ = -1;
					}
				}

			}
			else {
				if (selectNumber_ == -1) {
					if (ImGui::IsMouseClicked(0) and !ImGui::GetIO().WantCaptureMouse) {
						selectTexNumber_ = -1;
						selectTexImguiNumber_ = -1;
					}
				}
			}
		}
	}


}

//void TextureEditor::ShortCutMove(const SceneID id) {
//	bool controlLong = input_->GetKey()->LongPush(DIK_LCONTROL);
//	bool a_keyLong = input_->GetKey()->LongPush(DIK_A);
//	bool s_key = input_->GetKey()->Pushed(DIK_S);
//	bool l_key = input_->GetKey()->Pushed(DIK_L);
//	bool u_key = input_->GetKey()->LongPush(DIK_U);
//	bool i_key = input_->GetKey()->Pushed(DIK_I);
//
//	bool backSpace = input_->GetKey()->Pushed(DIK_BACKSPACE);
//	bool delete_key = input_->GetKey()->Pushed(DIK_DELETE);
//	//利用状況のモノだけは外す
//	if (u_key and i_key) {
//		if (isTextureEditor_) {
//			isTextureEditor_ = false;
//		}
//		else {
//			isTextureEditor_ = true;
//		}
//	}
//	if (isTextureEditor_.OnEnter()) {
//		isDecideToPlace_ = true;
//		isSelecteTex_ = true;
//	}
//
//	if (!isTextureEditor_) {
//		isDecideToPlace_ = false;
//		isSelecteTex_ = false;
//		isShortCuts_ = false;
//	}
//	//ショートカットが許可されていなければ即抜けるように
//	if (!isShortCuts_) {
//		return;
//	}
//
//	auto& texList = texies_[static_cast<size_t>(id)];
//	auto& texColorList = texColors_[static_cast<size_t>(id)];
//
//	//ショートカットキーの挙動
//	if (controlLong) {
//		//保存のショートカット
//		if (s_key) {
//			for (auto& i : sceneName_) {
//				if (i.first != id) {
//					continue;
//				}
//				if (OperationConfirmation()) {
//					SaveFile(i.second.c_str());
//				}
//				break;
//
//			}
//		}
//		//読み込みのショートカット
//		else if (l_key) {
//			for (auto& i : sceneName_) {
//				if (i.first != id) {
//					continue;
//				}
//				std::string file = (kDirectoryPath_ + i.second + ".json");
//				if (OperationConfirmation()) {
//					LoadFile(file.c_str());
//				}
//				break;
//			}
//
//		}
//		//全部のファイルに関連するショートカット
//		if (a_keyLong) {
//			//全保存
//			if (s_key) {
//				if (OperationConfirmation()) {
//					LoadFileAll();
//				}
//			}
//		}
//	}
//	if (a_keyLong) {
//		//全削除
//		if (backSpace or delete_key) {
//			if (OperationConfirmation()) {
//				texies_[static_cast<uint32_t>(id)].clear();
//				texColors_[static_cast<uint32_t>(id)].clear();
//			}
//		}
//
//	}
//	//削除
//	if (selectTexNumber_ != -1) {
//		if (backSpace or delete_key) {
//			if (OperationConfirmation()) {
//				texList.erase(texList.begin() + (size_t)(selectTexNumber_));
//				texColorList.erase(texColorList.begin() + (size_t)(selectTexNumber_));
//			}
//		}
//	}
//}

void TextureEditor::NewTexMake(const SceneID id, std::filesystem::path path) {
	//新しく設定するために用意
	std::unique_ptr<Tex2DState> setTex = std::make_unique<Tex2DState>();
	uint32_t color = 0xffffffff;
	setTex->color = color;
	setTex->transform = newTex_->transform;
	if (isUseTextureBaseSize_) {
		setTex->transform.scale_ = TextureManager::GetInstance()->GetTexture(TextureManager::Load(path.string().c_str()))->textureSize_;
	}
	setTex->angle_degrees = newTex_->angle_degrees;
	setTex->transform.rotate_ = DegreeToRadian(newTex_->angle_degrees);
	setTex->originalTransform = setTex->transform;
	setTex->sprite = Sprite::Generate(TextureManager::Load(path.string().c_str()));
	setTex->uvTransform.translate_ = setTex->sprite->GetUV().first;
	setTex->uvTransform.scale_ = setTex->sprite->GetUV().second;
	//setTex->blend = baseBlend_;
	setTex->textureFullPath = path.string();

	setTex->textureName = path.stem().string();
	texies_[static_cast<size_t>(id)].emplace_back(std::move(setTex));
	texColors_[static_cast<size_t>(id)].emplace_back(std::move(color));
}

void TextureEditor::ChangeTex(std::filesystem::path path, Tex2DState* tex) {
	tex->sprite->SetTextureHaundle(TextureManager::Load(path.string().c_str()));
	tex->textureFullPath = path.string();
	tex->textureName = path.stem().string();
}

void TextureEditor::DragMove([[maybe_unused]] std::filesystem::path path) {
#ifdef _DEBUG
	// ドラッグ処理の開始
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		dragSelectPath_ = path;
		isDraging_ = true;
		ImGui::Text("ドラッグ: %s", path.string().c_str());
		ImGui::EndDragDropSource();
	}
#endif
}

void TextureEditor::DropMove([[maybe_unused]] const SceneID id, [[maybe_unused]] Vector2 mousePos) {
#ifdef _DEBUG
	// ドロップされた場所で配置処理
	if (isDraging_) {
		if (ImGui::IsMouseDown(0)) {
			newTex_->transform.translate_ = mousePos;
		}
		if (ImGui::IsMouseReleased(0)) {

			NewTexMake(id, dragSelectPath_);
			isDraging_ = false;
		}
	}
#endif
}

bool TextureEditor::LoadChackItem(const std::string& fileName) {
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

bool TextureEditor::OperationConfirmation() {
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

void TextureEditor::from_json(const json& j, Vector2& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
}

void TextureEditor::from_json(const json& j, Vector3& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}


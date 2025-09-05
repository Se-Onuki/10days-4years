#pragma once
#include<Utils/Math/Transform.h>
#include<DirectBase/2D/Sprite.h>


struct Tex2DState {
	SoLib::Transform2D transform;//座標
	SoLib::Transform2D originalTransform;//イージングによる変化に対応するための保存用
	SoLib::Transform2D uvTransform;//uv座標
	std::unique_ptr<Sprite> sprite;
	int32_t angle_degrees;//度数法での表現
	std::string textureName;//テクスチャの名前
	std::string textureFullPath;//テクスチャのフルパス
	uint32_t color = 0xffffffff;//色
	//BlendType blend = BlendType::kNormal;//ブレンドタイプ
};

enum class SceneID {
	Title,
	StageSelect,
	Game,
	Result,

	kNum
};

inline Vector2 ImVec2toSolVec(const ImVec2 imvec2) {
	Vector2 result{};

	result = { imvec2.x,imvec2.y };

	return result;
}

inline SoLib::Angle::Radian DegreeToRadian(int32_t degree) {
    return SoLib::Angle::Radian(degree * (std::numbers::pi_v<float> / 180.0f));
}

inline std::vector<std::filesystem::path> GetFilePathFormDir(
    const std::filesystem::path& directoryName,
    const std::filesystem::path& extension
) {
    std::vector<std::filesystem::path> result;
    if (not std::filesystem::exists(directoryName)) {
        return result;
    }

    std::filesystem::directory_iterator directory{ directoryName };

    // 基準にしたいルートパス
    std::filesystem::path base = "./resources/";

    // ディレクトリ内を探索
    for (const auto& entry : directory) {
        const auto& path = entry.path();
        // サブディレクトリを探索
        if (!path.has_extension()) {
            auto files = GetFilePathFormDir(path, extension);
            result.insert(result.end(), files.begin(), files.end());
        }
        // 特定の拡張子を持つファイルを追加
        else if (path.extension() == extension) {
            // ./resources/ からの相対パスに変換
            result.push_back(std::filesystem::relative(path, base));
        }
    }

    return result;
}

inline std::vector<std::filesystem::path> GetFilePathFormDirOrigin(
    const std::filesystem::path& directoryName, const std::filesystem::path& extension) {
    std::vector<std::filesystem::path> result; 
    if (not std::filesystem::exists(directoryName)) { 
        return result; 
    }

    std::filesystem::directory_iterator directory{ directoryName }; 
    
    // ディレクトリ内を探索 
    for (const auto& entry : directory) { 
        const auto& path = entry.path(); 
         // もし拡張子がなかったら
        if (!path.has_extension()) { 
            std::vector<std::filesystem::path> files; 
            // その中にあるファイルをすべて探索(再帰的に探す) 
            files = GetFilePathFormDirOrigin(path, extension);
            // コンテナに追加 
            for (auto& i : files) { 
                result.push_back(i); 
            } 
        } 
            // 特定の拡張子を持つファイルだった場合コンテナに追加 
        else if (path.extension() == extension) { 
            result.push_back(path); 
        } 
    } 
    return result; 
}


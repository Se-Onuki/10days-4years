#pragma once
#include "Utils/Containers/Singleton.h"		// シングルトン用のクラス
#include<../User/AoMidori.h>
//セレクトで選んだステージ番号だったりの受け渡し

class SelectToGame : public SoLib::Singleton<SelectToGame> {
public:
	SelectToGame() = default;
	SelectToGame(const SelectToGame &) = delete;
	SelectToGame(SelectToGame &&) = delete;
	~SelectToGame();

	SelectToGame &operator=(const SelectToGame &) = delete;
	SelectToGame &operator=(SelectToGame &&) = delete;

	friend SoLib::Singleton<SelectToGame>;
public:

	/// @brief ステージ番号を取得する
	/// @return ステージ番号(int32_t型)を返す
	const int32_t GetStageNum() const {
		return stageNum_;
	}
	/// @brief ステージ番号を取得する
	/// @return ステージ番号(int32_t型)を返す
	const float GetCameraScale() const {
		return cameraScale_;
	}

	/// @brief ステージ番号を設定する
	/// @param[in] num 設定するステージ番号
	void SetStageNum(const int32_t num) {
		stageNum_ = num;
	}

	/// @brief ステージ番号を設定する
	/// @param[in] num 設定するステージ番号
	void SetCameraScale(const float num) {
		cameraScale_ = num;
	}


private:
	//ステージ番号
	int32_t stageNum_ = 0;
	//良い場所が浮かばなかったので究極的なとりあえずここに
	float cameraScale_ = 0.0125f;
};


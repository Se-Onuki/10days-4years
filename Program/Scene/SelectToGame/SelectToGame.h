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
	/// @brief ステージ最大値を取得する
	/// @return ステージ最大値(int32_t型)を返す
	const int32_t GetStageMax() const {
		return stageMax_;
	}
	/// @brief ステージ番号を取得する
	/// @return ステージ番号(int32_t型)を返す
	const float GetCameraScale() const {
		return cameraScale_;
	}

	bool GetClearFlug()const {
		return isClear_;
	}

	/// @brief ステージ番号を設定する
	/// @param[in] num 設定するステージ番号
	void SetStageNum(const int32_t num) {
		stageNum_ = num;
	}

	/// @brief ステージ最大値を設定する
	/// @param[in] num 設定するステージ最大値
	void SetStageMax(const int32_t num) {
		stageMax_ = num;
	}

	/// @brief ステージ番号を設定する
	/// @param[in] num 設定するステージ番号
	void SetCameraScale(const float num) {
		cameraScale_ = num;
	}

	void SetClearFlug(bool flug) {
		isClear_ = flug;
	}


private:
	//ステージ番号
	int32_t stageNum_ = 0;

	int32_t stageMax_ = 0;

	bool isClear_ = false;
	//良い場所が浮かばなかったので究極的なとりあえずここに
	float cameraScale_ = 0.0125f;
};


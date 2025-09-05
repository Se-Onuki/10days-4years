#pragma once
#include<../User/AoMidori.h>
//セレクトで選んだステージ番号だったりの受け渡し

class SelectToGame{
public:
	SelectToGame() = default;
	SelectToGame(const SelectToGame&) = delete;
	SelectToGame(SelectToGame&&) = delete;
	~SelectToGame();

	SelectToGame& operator=(const SelectToGame&) = delete;
	SelectToGame& operator=(SelectToGame&&) = delete;
public:


	static SelectToGame* const GetInstance() {
		static SelectToGame instance;
		return &instance;
	}

	const int32_t GetStageNum() const {
		return stageNum_;
	}

	void SetStageNum(const int32_t num) {
		stageNum_ = num;
	}


private:
	//ステージ番号
	int32_t stageNum_ = 0;

};


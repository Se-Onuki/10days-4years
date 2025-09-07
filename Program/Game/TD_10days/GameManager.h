#pragma once
#include <editor/StageEditor.h>

namespace TD_10days {

	class GameManager;

	class ResetAbleGameObject {
		friend GameManager;
	public:

	private:
	};

	class GameManager {
	public:

		void Init();

		void Update(const float deltaTime);

		void Draw() const;

	private:

		// レベルエディタのポインタ
		StageEditor *pStageEditor_ = nullptr;

		// レベルのマップチップ
		TD_10days::LevelMapChip *pLevelMapChip_;
		// レベルのマップチップの当たり判定
		const TD_10days::LevelMapChip::LevelMapChipHitBox *pLevelMapChipHitBox_;
		// マップチップの描画クラス
		std::unique_ptr<TD_10days::LevelMapChipRenderer> levelMapChipRenderer_;
	};

}
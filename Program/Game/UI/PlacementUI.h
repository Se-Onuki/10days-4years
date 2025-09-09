#pragma once
#include "../../../Engine/DirectBase/2D/Sprite.h"

namespace TD_10days {
	class PlacementUI {
	public:
		enum class EasingState {
			kNone,       // 何もしない
			kAppearing,  // 出現中
			kDisappearing // 消失中
		};

		enum class ArrowState {
			kNone,
			kTrigger,
			kIncapable
		};

		struct Arrow {
			std::vector<std::unique_ptr<Sprite>> sprites_;
			ArrowState state_ = ArrowState::kNone;
			float pressTimer = 0.0f;
			const float pressDuration = 0.2f;
		};

		enum class Direction {
			kRight,
			kUp,
			kLeft,
			kDown
		};

	public:

		PlacementUI() = default;
		~PlacementUI() = default;
		void Init(const Vector2& basePos);
		void Update(float deltaTime);
		void Draw();

		void Appear() { easingState_ = EasingState::kAppearing; elapsed_ = 0.0f; };
		void Disappear() { easingState_ = EasingState::kDisappearing; elapsed_ = 0.0f; };

		void SetIsPlaceableDir_(Direction dir, bool isPlaceable) { isPlaceableDir_[static_cast<int>(dir)] = isPlaceable; }
		void SetBasePos(const Vector2& pos) { basePos_ = pos; }
		const Vector2& GetBasePos() const { return basePos_; }
		void SetActive(const bool isActive) { isActive_ = isActive; }
		bool IsActive() const { return isActive_; }

	private:
	private:
		bool isActive_ = false;
		Vector2 basePos_{};
		const float offset_ = 1.0f;
		const float uiOffset_ = 2.0f;

		bool isPlaceableDir_[4]{ false, false, false, false };

		// イージング
		EasingState easingState_ = EasingState::kNone;
		const float appearTime_ = 0.3f;
		const float disappearTime_ = 0.3f;
		float elapsed_ = 0.0f; // アニメーション進行度

		// 矢印
		std::vector<Arrow> arrows_;
		const Vector2 arrowSize_{ 1.0f, 1.0f };
		const Vector2 texSize_{ 80.0f, 80.0f };

		// テキスト
		std::unique_ptr<Sprite> text_;
		const Vector2 textSize_{ 1.5f, 0.5f };

		// ボタン
		std::unique_ptr<Sprite> button_;
		const Vector2 buttonSize_{ 0.5f, 0.5f };
	};
}
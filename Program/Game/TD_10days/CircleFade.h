#pragma once
/// @file Fade.h
/// @brief フェードクラス
/// @author ONUKI seiya
#pragma once
#include <stdint.h>
#include <memory>

#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include "../../Engine/Utils/SoLib/SoLib_Easing.h"

#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/DirectBase/2D/Sprite.h"

#include "../../Engine/Utils/Graphics/Color.h"

namespace TD_10days {
	class CircleFade {
        CircleFade() = default;
        CircleFade(const CircleFade&) = delete;
        CircleFade& operator=(const CircleFade&) = delete;
        ~CircleFade() = default;

    public:
        static CircleFade* const GetInstance() {
            static CircleFade instance{};
            return &instance;
        }

        /// @brief 初期化（行数・列数・円の直径を指定）
        static void StaticInit(int rows, int cols, float circleSize);

        /// @brief フェード開始
        /// @param goalTime 終了までの時間
        /// @param isClosing trueなら円を拡大して閉じる / falseなら縮小して開く
        void Start(float goalTime, bool isClosing);

        void Update(float deltaTime);

        void Draw();

        /// @brief イージング関数の設定
        void SetEaseFunc(float (*easeFunc)(float)) { easeFunc_ = easeFunc; }

        /// @brief タイマーの取得
        const SoLib::DeltaTimer* const GetTimer() { return timer_.get(); }

    private:
        /*std::vector<std::unique_ptr<Sprite>> sprites_;
        std::unique_ptr<SoLib::DeltaTimer> timer_;
        std::function<float(float)> easeFunc_ = SoLib::easeLinear;
        bool isClosing_ = true;*/

        std::vector<std::unique_ptr<Sprite>> sprites_;
        std::unique_ptr<SoLib::DeltaTimer> timer_;
        std::function<float(float)> easeFunc_ = SoLib::easeLinear;
        bool isClosing_ = true;
        int rows_ = 0;
        int cols_ = 0;
        float maxScale_ = 0.0f;

        bool firstRun_ = true;
	};
}


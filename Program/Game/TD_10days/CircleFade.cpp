#include "CircleFade.h"

namespace TD_10days {
    void CircleFade::StaticInit(int rows, int cols, float circleSize)
    {
        //auto* const instance = GetInstance();
        //instance->timer_ = std::make_unique<SoLib::DeltaTimer>();
        //instance->timer_->Clear();

        //auto* const winApp = WinApp::GetInstance();
        //const float width = static_cast<float>(winApp->kWindowWidth);
        //const float height = static_cast<float>(winApp->kWindowHeight);

        //instance->sprites_.clear();
        //instance->sprites_.reserve(rows * cols);

        //// 格子状に円を配置
        //for (int y = 0; y < rows; ++y) {
        //    for (int x = 0; x < cols; ++x) {
        //        std::unique_ptr<Sprite> circle = Sprite::Generate(TextureManager::Load("splash.png"));
        //        circle->SetColor(SoLib::Color::RGB4(0, 0, 0, 255));
        //        circle->SetPivot({ 0.5f, 0.5f });
        //        circle->SetScale({ circleSize, circleSize });

        //        float posX = (x + 0.5f) * (width / cols);
        //        float posY = (y + 0.5f) * (height / rows);
        //        circle->SetPosition({ posX, posY });

        //        instance->sprites_.push_back(std::move(circle));
        //    }
        //}

        auto* const instance = GetInstance();
        instance->timer_ = std::make_unique<SoLib::DeltaTimer>();
        instance->timer_->Clear();

        auto* const winApp = WinApp::GetInstance();
        const float width = static_cast<float>(winApp->kWindowWidth);
        const float height = static_cast<float>(winApp->kWindowHeight);

        instance->rows_ = rows;
        instance->cols_ = cols;
        instance->maxScale_ = std::sqrt(width * width + height * height);

        instance->sprites_.clear();
        instance->sprites_.reserve(rows * cols);

        float cellW = width / cols;
        float cellH = height / rows;
        //float autoCircleSize = max(cellW, cellH);

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                auto circle = Sprite::Generate(TextureManager::Load("Player-export.png"));
                circle->SetColor(SoLib::Color::RGB4(0, 0, 0, 255));
                circle->SetPivot({ 0.5f, 0.5f });
               /* circle->SetScale({ circleSize > 0 ? circleSize : autoCircleSize,
                                   circleSize > 0 ? circleSize : autoCircleSize });*/
                circle->SetScale({circleSize, circleSize});

                float posX = (x + 0.5f) * cellW;
                float posY = (y + 0.5f) * cellH;
                circle->SetPosition({ posX, posY });

                instance->sprites_.push_back(std::move(circle));
            }
        }
    }

    void CircleFade::Start(float goalTime, bool isClosing) {
        if (firstRun_) {
            if (isClosing) {
                isClosing_ = isClosing;
                timer_->Clear();
                timer_->Start(goalTime);

                float startScale = isClosing_ ? 0.0f : maxScale_;
                for (auto& s : sprites_) {
                    s->SetScale({ startScale, startScale });
                }
                firstRun_ = false;
            }
        }
        else {
            isClosing_ = isClosing;
            timer_->Clear();
            timer_->Start(goalTime);

            float startScale = isClosing_ ? 0.0f : maxScale_;
            for (auto& s : sprites_) {
                s->SetScale({ startScale, startScale });
            }
        }

       
    }

    //void CircleFade::Update(float deltaTime) {
    //   
    //    if (timer_->Update(deltaTime)) {
    //        float baseT = easeFunc_(timer_->GetProgress());
    //        const float delayScale = 0.2f; // 遅延割合

    //        for (int i = 0; i < sprites_.size(); i++) {
    //            auto& circle = sprites_[i];

    //            int colIndex = i % cols_;
    //            float delay = static_cast<float>(colIndex) / (cols_ - 1);

    //            float t = 0.0f;
    //            if (isClosing_) {
    //                // フェードアウト（左から覆う）
    //                float localT = baseT - delay * delayScale;
    //                t = localT / (1.0f - delayScale);   // 各列が同じ速度で拡大
    //            }
    //            else {
    //                // フェードイン（右から消す）
    //                float localT = baseT - (1.0f - delay) * delayScale;
    //                t = localT / (1.0f - delayScale);   // 各列が同じ速度で縮小
    //            }

    //            t = std::clamp(t, 0.0f, 1.0f);

    //            float scale = isClosing_
    //                ? std::lerp(0.0f, maxScale_, t)   // 左から拡大
    //                : std::lerp(maxScale_, 0.0f, t);  // 右から縮小

    //            circle->SetScale({ scale, scale });
    //        }

    //        if (timer_->IsFinish()) {
    //            timer_->Clear();
    //        }
    //    }

    //}

    void CircleFade::Update(float deltaTime) {

        if (timer_->Update(deltaTime)) {

            float baseP = easeFunc_(timer_->GetProgress());

            // 遅延の最大割合（0..1）。perColumnDelay_ を調整して演出の波を変える
            const float maxDelay = 0.2f;
            const float denom = 1.0f - maxDelay;
            const bool safeNormalize = denom > 1e-6f;

            for (int i = 0; i < static_cast<int>(sprites_.size()); ++i) {
                auto& circle = sprites_[i];

                int colIndex = i % cols_;
                // フェードイン（開く＝右から消す）は列を反転させるのと同等の扱いにする
                if (!isClosing_) {
                    colIndex = (cols_ - 1) - colIndex;
                }

                float delay = (cols_ > 1) ? (static_cast<float>(colIndex) / static_cast<float>(cols_ - 1)) : 0.0f;
                float startTime = delay * maxDelay;

                // ローカル進行を正規化（delay を引いて残り時間で割る）
                float localRaw;
                if (safeNormalize) {
                    localRaw = (baseP - startTime) / denom;
                }
                else {
                    localRaw = baseP; // maxDelay がほぼ 1 の場合のフォールバック
                }
                localRaw = std::clamp(localRaw, 0.0f, 1.0f);

                // 各列に対してイージングを適用（ここで同じイージングを使うことで左右で速度差が出ない）
                float t = easeFunc_(localRaw);

                // スケールを算出（拡大／縮小の方向は isClosing_ で決める）
                float scale = isClosing_
                    ? std::lerp(0.0f, maxScale_, t)   // フェードアウト: 小 -> 大（左から覆う）
                    : std::lerp(maxScale_, 0.0f, t);  // フェードイン: 大 -> 小（右から消す）

                circle->SetScale({ scale, scale });
            }

            // 完了フレームで確実に最終状態へスナップ
            if (timer_->IsFinish()) {
                float finalScale = isClosing_ ? maxScale_ : 0.0f;
                for (auto& c : sprites_) c->SetScale({ finalScale, finalScale });

                timer_->Clear();
            }
        }

        
    }

    void CircleFade::Draw() {
        for (auto& circle : sprites_) {
            circle->Draw();
        }
    }
}



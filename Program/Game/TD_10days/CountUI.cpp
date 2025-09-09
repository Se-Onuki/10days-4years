#include "CountUI.h"


namespace TD_10days {
    void CountUI::Init() {
        // 数字スプライトの読み込み（0〜9の画像を用意しておく）
        for (int i = 0; i <= 9; ++i) {
            std::unique_ptr<Sprite> onesNumber = Sprite::Generate(TextureManager::Load("WaterBreakNumber.png"));
            onesNumber->SetPivot({ 0.5f, 0.5f });
            onesNumber->SetScale({ 0.4f, 0.4f });
            onesNumber->SetInvertY(true);
            onesNumber->SetTexOrigin(Vector2{ 250.0f * i, 0.0f });
            onesNumber->SetTexDiff(Vector2{ 250.0f, 250.0f });

            onesNumber_.emplace_back(std::move(onesNumber));

            std::unique_ptr<Sprite> tensNumber = Sprite::Generate(TextureManager::Load("WaterBreakNumber.png"));
            tensNumber->SetPivot({ 0.5f, 0.5f });
            tensNumber->SetScale({ 0.4f, 0.4f });
            tensNumber->SetInvertY(true);
            tensNumber->SetTexOrigin(Vector2{ 250.0f * i, 0.0f });
            tensNumber->SetTexDiff(Vector2{ 250.0f, 250.0f });

            tensNumber_.emplace_back(std::move(tensNumber));


        }

    }

    void CountUI::Update(float deltaTime, const Vector2& position) {

        position_ = position;

        // アニメーションタイマー進行
        if (animTimer_ < animDuration_) {
            animTimer_ += deltaTime;
            if (animTimer_ > animDuration_) {
                animTimer_ = animDuration_;
            }
        }
    }

    void CountUI::Draw() {
        // もし値が無効であればその場で終わらせる
        if (time_ <= 0.f) { return; }

        //// 残り時間を整数にして表示
        int displayNumber = static_cast<int>(std::ceil(time_));

        int tens = displayNumber / 10; // 十の位
        int ones = displayNumber % 10; // 一の位

        // 線形補間でスケール計算
        float t = animTimer_ / animDuration_;
        float currentScale = SoLib::Lerp(0.0f, size_, t);

        Vector2 drawPos = position_ + offset;

        // 十の位描画（0は非表示でもOK）
        if (tens > 0) {
            tensNumber_[tens]->SetScale(Vector2{ currentScale, currentScale });
            tensNumber_[tens]->SetPosition(drawPos + Vector2{-0.25f, 0.0f});
            tensNumber_[tens]->Draw();
            drawPos += Vector2{ 0.25f, 0.0f };
        }

        // 一の位描画
        onesNumber_[ones]->SetScale(Vector2{ currentScale, currentScale });
        onesNumber_[ones]->SetPosition(drawPos);
        onesNumber_[ones]->Draw();
        
    }

    void CountUI::SetTime(float time)
    {
        time_ = time;

        int currentNumber = static_cast<int>(std::ceil(time_));
        if (currentNumber != prevNumber_) {
            // 数字が変わったらアニメーションをリセット
            animTimer_ = 0.f;
            prevNumber_ = currentNumber;
        }

    }

}
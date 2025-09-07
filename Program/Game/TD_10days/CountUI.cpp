#include "CountUI.h"


namespace TD_10days {
    void CountUI::Init() {
        // 数字スプライトの読み込み（0〜9の画像を用意しておく）
        for (int i = 0; i <= 9; ++i) {
            std::unique_ptr<Sprite> sprite = Sprite::Generate(TextureManager::Load("WaterBreakNumber.png"));
            sprite->SetPivot({ 0.5f, 0.5f });
            sprite->SetScale({ 0.4f, 0.4f });
            sprite->SetInvertY(true);
            sprite->SetTexOrigin(Vector2{ 250.0f * i, 0.0f });
            sprite->SetTexDiff(Vector2{ 250.0f, 250.0f });


            number_.emplace_back(std::move(sprite));
        }

        //time_ = count_; // 初期残り時間
        isActive_ = false;
    }

    void CountUI::Update(float deltaTime, const Vector2& position) {
        if (not isActive_) { return; };

        time_ -= deltaTime;
        if (time_ < 0.0f) {
            time_ = 0.0f;
            isActive_ = false; // カウント終了
        }
        position_ = position;
    }

    void CountUI::Draw() {
        if (not isActive_) { return; };

        //// 残り時間を整数にして表示
        int displayNumber = static_cast<int>(std::ceil(time_));

        number_[displayNumber]->SetPosition(position_ + offset);
        number_[displayNumber]->Draw();
        
    }

}
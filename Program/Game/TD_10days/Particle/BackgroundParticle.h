#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>
#include <list>
#include <algorithm>

#include "../Engine/DirectBase/Input/Input.h"
#include "../../Engine/DirectBase/Base/TextureManager.h"	// テクスチャの情報
#include "../../Engine/Utils/Math/Random.h"
#include "../../Engine/DirectBase/File/VariantItem.h"		// 外部編集可能な変数の定義
#include "../../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraManager.h"

namespace TD_10days {
    class BackgroundParticle {
    public:
        void Init(const SolEngine::Camera2D& camera);
        void Update(float deltaTime);
        void Draw();

        void SetCamera(const SolEngine::Camera2D* camera) { pCamera_ = camera; }

    private:
        const SolEngine::Camera2D* pCamera_;
        std::unique_ptr<Sprite> sprite_;
        Vector2 position_;
        Vector2 velocity_;

        float age_ = 0.0f;
        float lifeTime_ = 3.0f;

        Vector4 startColor_;
        Vector4 midColor_;
        Vector4 endColor_;

        bool isActive_ = false;
    };

}


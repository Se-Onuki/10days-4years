#include "Player.h"
#include "../../Header/Collision/Collision.h"
#include "../../../Engine/DirectBase/Input/Input.h"

namespace TD_10days {

	void Player::Init() {
		sprite_ = Sprite::Generate(TextureManager::Load(spriteName_));
		sprite_->SetPivot(Vector2::one / 2.f);
		sprite_->SetInvertY(true);

	}

	void Player::Update([[maybe_unused]] const float deltaTime) {
		// 移動処理
		MoveUpdate(deltaTime);



		// スプライトの計算
		CalcSprite();
	}

	void Player::Draw() const {
		sprite_->Draw();
	}

	void Player::InputFunc() {
		const auto input = SolEngine::Input::GetInstance();
		const auto dInput = input->GetDirectInput();
		const float moveSpeed = 5.f;
		if (dInput->IsPress(DIK_A)) {
			velocity_.x -= moveSpeed;
		}
		if (dInput->IsPress(DIK_D)) {
			velocity_.x += moveSpeed;
		}

		if (dInput->IsTrigger(DIK_SPACE)) {
			if (isGround_ or IsInWater()) {
				velocity_.y = 0.f;
				acceleration_.y += 5.f;
			}
		}

		if (dInput->IsTrigger(DIK_RETURN)) {
			Vector2 place = Vector2{ std::roundf(position_.x), std::roundf(position_.y) };
			pWater_->Init(place, Vector2::one, 0x0000FF55);
		}

		Vector2 nextDir = Vector2::zero;

		if (dInput->IsTrigger(DIK_RIGHT)) {
			nextDir += +Vector2::right;
		}
		if (dInput->IsTrigger(DIK_LEFT)) {
			nextDir += -Vector2::right;
		}
		if (dInput->IsTrigger(DIK_UP)) {
			nextDir += +Vector2::up;
		}
		if (dInput->IsTrigger(DIK_DOWN)) {
			nextDir += -Vector2::up;
		}

		if (nextDir.LengthSQ() == 1.f) {
			if (pWater_->IsPlaceAble(pHitBox_, nextDir)) {
				pWater_->PlacementWater(nextDir);
			}
		}
	}

	void Player::CalcSprite() {
		// 座標合わせ
		sprite_->SetPosition(position_);
	}

	std::tuple<float, Vector3>  Player::CalcMoveProgress(const Vector2 &velocity)
	{
		if (velocity == Vector2::zero) {
			return { 1.f,Vector3::zero };
		}
		// プレイヤの中心から4点分の座標を計算
		const Vector2 halfSize = size_ / 2;
		const std::array<Vector2, 4> playerPoints = {
			Vector2{-.5f, -.5f}, // 左下
			Vector2{-.5f, +.5f}, // 左上
			Vector2{+.5f, -.5f}, // 右下
			Vector2{+.5f, +.5f}  // 右上
		};

		//// AABBを作成し､その範囲内にある当たり判定を調べる
		//AABB playerAABB = AABB::Create(Vector3(position_.x, position_.y), Vector3(halfSize.x, halfSize.y));
		//playerAABB.Extend(Vector3(velocity.x, velocity.y));

		// 衝突しているかをレイキャストで調べる
		LineBase segment{};
		segment.lineType = LineBase::LineType::Segment;
		segment.origin = Vector3::zero;
		segment.diff = Vector3(velocity.x, velocity.y, 0.f) * Quaternion::AnyAxisRotation(Vector3::front, 0.1_deg);

		float minProgress = 1.f;
		Vector3 hitNormal = Vector3::zero;

		// ヒットボックスに対して判定を行う
		for (int32_t yi = 0; yi < static_cast<int32_t>(pHitBox_->GetY()); ++yi) {
			for (int32_t xi = 0; xi < static_cast<int32_t>(pHitBox_->GetX()); ++xi) {
				// 当たり判定が無いなら無視
				if (not pHitBox_->at(yi, xi)) {
					continue;
				}

				// ヒットボックスの作成
				AABB hitBox = AABB::Create(
					Vector3(static_cast<float>(xi), static_cast<float>(yi), 0.f),
					Vector3::one * 0.51f
				);

				// プレイヤの各点からレイキャスト
				for (const auto &point : playerPoints) {
					const Vector2 worldPoint = Vector2(point.x * size_.x, point.y * size_.y) + position_;
					segment.origin = Vector3(worldPoint.x, worldPoint.y, 0.f);


					// 衝突しているかを調べる
					if (Collision::IsHit(hitBox, segment)) {
						// 衝突しているなら進行度を計算
						const float progress = Collision::HitProgress(segment, hitBox);
						// もし裏面に衝突していたら無視
						const Vector3 hitPoint = segment.GetProgress(progress);

						/// 箱の中心点から接触点へのベクトル

						const Vector2 boxCenter = Vector2(static_cast<float>(xi), static_cast<float>(yi));
						const Vector2 toHitPoint = hitPoint.ToVec2() - boxCenter;

						// もし接触点と移動ベクトルが同じ向きなら無視
						if ((toHitPoint * velocity) > 0.f) {
							continue;
						}

						const Vector2 toHitPosSign = Vector2(std::copysign(1.f, toHitPoint.x), std::copysign(1.f, toHitPoint.y));

						// 接触点へのベクトルとプレイヤの頂点ベクトルの和
						const Vector2 dotVec = Vector2(toHitPosSign.x + point.x * 2.f, toHitPosSign.y + point.y * 2.f);

						// 接触面の法線を取得
						auto normalList = hitBox.GetNormalList(hitPoint);

						// 角同士が衝突した場合
						if (dotVec == Vector2::zero) {
							// もし法線が1つしかない場合かつ､頂点に近似している場合､もう片方の法線も追加する
							if (normalList.size() == 1.f and std::fabsf(std::fabsf(toHitPoint.x) - std::fabsf(toHitPoint.y)) < 0.05f) {
								const auto vec = (toHitPosSign - normalList.front().ToVec2());
								normalList.emplace_back(vec.x, vec.y, 0.f);
							}

							if (normalList.size() > 1.f) {

								// 隣のブロックを参照して残す法線を決定する
								std::erase_if(normalList, [&](const Vector3 &normal) {
									// 隣のブロックへの相対座標
									const Vector2 diff = toHitPosSign - normal.ToVec2();
									// 隣のブロックの座標
									const Vector2 checkPos = boxCenter + diff;

									// その場所にブロックがあるか
									const bool isHitBox = checkPos.x >= 0.f and checkPos.y >= 0.f and pHitBox_->at(static_cast<size_t>(checkPos.y), static_cast<size_t>(checkPos.x));
									// 無かったらその法線を無効化する
									return not isHitBox;


									}
								);
							}
						}

						for (Vector3 normal : normalList) {
							for (uint32_t i = 0; i < 2; ++i) {
								// 0以外であればその法線は無効化
								if (dotVec.cdata()[i] != 0.f) {
									normal[i] = 0.f;
								}
							}
							if (normal.ToVec2() == Vector2::zero) {
								continue;
							}

							// 法線が進行方向と逆を向いていなければ無視
							const float dot = (Vector3(velocity.x, velocity.y, 0.f) * normal);
							if (dot >= 0.f) {
								continue;
							}

							if (minProgress > progress) {
								hitNormal = normal;
								minProgress = progress;
							}
						}

					}
				}
			}
		}

		return { minProgress, hitNormal };

	}

	void Player::MoveUpdate(float deltaTime) {
		acceleration_ += gravity_ * deltaTime;

		velocity_ += acceleration_;
		std::list<Vector3> hitNormalList;

		Vector2 moveVec = velocity_ * deltaTime;

		const auto [progress, hitNormal] = CalcMoveProgress(moveVec);
		hitNormalList.emplace_back(hitNormal);

		// 接触するまでの移動
		position_ += moveVec * (progress);

		// もし0.fで接触したなら
		if (progress <= 0.f) {

			// 次の移動量
			moveVec = moveVec.Reflect(hitNormal.ToVec2(), 0.f) * (1.f - progress);

			const auto [nextProgress, nextHitNormal] = CalcMoveProgress(moveVec);
			hitNormalList.emplace_back(nextHitNormal);

			// 接触した後の移動
			position_ += moveVec * nextProgress;
		}

		isGround_ = false;

		if (std::find(hitNormalList.begin(), hitNormalList.end(), Vector3::up) != hitNormalList.end()) {

			velocity_.y = 0.f;
			isGround_ = true;
		}

		velocity_.x = 0.f;
		acceleration_ = Vector2::zero;
	}

	bool Player::IsInWater() const
	{
		const auto waterPos = pWater_->GetWaterPosition();

		const Vector2 target = Vector2{ std::roundf(position_.x), std::roundf(position_.y) };

		return std::find(waterPos.begin(), waterPos.end(), target) != waterPos.cend();
	}

}

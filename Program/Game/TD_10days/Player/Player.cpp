#include "Player.h"
#include "../../Header/Collision/Collision.h"
#include "../../../Engine/DirectBase/Input/Input.h"

namespace TD_10days {

	void Player::Init() {
		sprite_ = Sprite::Generate(TextureManager::Load("uvChecker.png"));
		sprite_->SetPivot(Vector2::one / 2.f);
		sprite_->SetScale(size_);
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
		if (dInput->IsPress(DIK_W)) {
			velocity_.y += moveSpeed;
		}
		if (dInput->IsPress(DIK_S)) {
			velocity_.y -= moveSpeed;
		}
		if (dInput->IsPress(DIK_A)) {
			velocity_.x -= moveSpeed;
		}
		if (dInput->IsPress(DIK_D)) {
			velocity_.x += moveSpeed;
		}

		if (velocity_.x * velocity_.y != 0.f) {
			velocity_ *= 0.7071f;
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
					Vector3::one / 2.f
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
							// 隣のブロックを参照して残す法線を決定する
							std::erase_if(normalList,[&](const Vector3 &normal) {
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
		//velocity_ += acceleration_ * deltaTime;

		const Vector2 moveVec = velocity_ * deltaTime;

		const auto [progress, hitNormal] = CalcMoveProgress(moveVec);

		// 接触するまでの移動
		position_ += moveVec * (progress);

		// もし0.fで接触したなら
		if (progress <= 0.f) {

			// 次の移動量
			const Vector2 nextMove = moveVec.Reflect(hitNormal.ToVec2(), 0.f) * (1.f - progress);

			const auto [nextProgress, nextHitNormal] = CalcMoveProgress(nextMove);

			// 接触した後の移動
			position_ += nextMove * nextProgress;
		}
		velocity_ = Vector2::zero;
		//acceleration_ = Vector2::zero;
	}

}

#include "Player.h"
#include "ImGui.h"
 #include <cmath>
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);
	assert(modelAttack);

	// 引数の内容をメンバ変数に記録
	model_ = model;
	modelAttack_ = modelAttack;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransformAttack_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
}

KamataEngine::Vector3 Player::CornerPositon(const KamataEngine::Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    Vector3{+kWidth / 2.0f, -kHeight / 2.0f, 0}, // 右下
	    Vector3{-kWidth / 2.0f, -kHeight / 2.0f, 0}, // 左下
	    Vector3{+kWidth / 2.0f, +kHeight / 2.0f, 0}, // 右上
	    Vector3{-kWidth / 2.0f, +kHeight / 2.0f, 0}, // 左上
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::Move() {

	// 移動入力
	// 接地状態
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右処理
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// 左移動中の右入力
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_ += acceleration;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {

			// 非入力時は移動減速をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ処理
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
		// 空中
	} else {

		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFailSpeed);
	}

	if (turnTimer_ > 0.0f) {

		// 旋回タイマーをカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 旋回制御
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float>, std::numbers::pi_v<float> * 2.0f};
		// 状況にあった角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 正規化された時間（0.0～1.0）を計算
		float t = 1.0f - (turnTimer_ / kTimeTurn);

		// イージング補間値を取得
		float easedT = easeInOutCubic(t);

		// 開始角度から終了角度へイージング補間
		float currentRotationY = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * easedT;

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = currentRotationY;
	}
}

void Player::CollisionTop(CollisionMapInfo& info) {

	// 上昇あり？
	if (info.velocity.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositon(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// (自キャラの)左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		 indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		 MapChipField::IndexSet indexSetNow;
		 indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		
		 if (indexSetNow.yIndex != indexSet.yIndex) {
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//float newTopY = positionsNew[kRightTop].y - kHeight / 2.0f;
		info.velocity.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		//info.velocity.y = std::max(0.0f, rect.bottom - newTopY + kBlank);
		DebugText::GetInstance()->ConsolePrintf("info.move.y = %f", rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f));

		// 天井に当たったことを記録する
		info.ceiling = true;
		}
	}
}

void Player::CollisionBottom(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositon(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	// 下降あり？

	if (info.velocity.y >= 0) {
		return;
	}
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kLandingAdjust, 0));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kLandingAdjust, 0));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		 indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		 MapChipField::IndexSet indexSetNow;
		 indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		
		 if (indexSetNow.yIndex != indexSet.yIndex) {

		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.landing = true;
		}
	}
}

void Player::CollisionRight(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositon(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}

	// 右移動あり？
	if (info.velocity.x <= 0.0f) {
		return;
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// 右上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		 indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		 MapChipField::IndexSet indexSetNow;
		 indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		//// めり込み先ブロックの範囲矩形
		//
		 if (indexSetNow.xIndex != indexSet.xIndex) {
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// info.move.x = std::min(0.0f, rect.left - (worldTransform_.translation_.x + (kWidth / 2.0f + kBlank)));
		info.velocity.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		// 壁に当たったことを記録する
		// DebugText::GetInstance()->ConsolePrintf("landing");
		info.hitWall = true;
		}
	}
}

void Player::CollisionLeft(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositon(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}

	// 左移動あり？
	if (info.velocity.x >= 0.0f) {
		return;
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// 左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {

indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		 MapChipField::IndexSet indexSetNow;
		 indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		
		 if (indexSetNow.xIndex != indexSet.xIndex) {
		//  めり込みを排除する方向に移動量を設定する
		
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		// 壁に当たったことを記録する
		// DebugText::GetInstance()->ConsolePrintf("landing");
		info.hitWall = true;
		}
	}
}

void Player::SwitchLanding(const CollisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		std::array<Vector3, kNumCorner> positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositon(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
		}
		MapChipType mapChipType;
		// 真上のあたり判定を行う
		bool hit = false;
		// 左下の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
		// 右下の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}

		// 落下開始
		if (!hit) {
			// 空中状態に切り替える
			onGround_ = false;
		}

	} else {

		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX座標を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

void Player::CollisionWall(const CollisionMapInfo& info) {

	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::Collision(CollisionMapInfo& info) {
	CollisionBottom(info);
	CollisionTop(info);
	CollisionLeft(info);
	CollisionRight(info);
}

void Player::CollisionMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.velocity;
}

void Player::CollisionUpdate(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

void Player::BehaviorRootinitialize() {}

void Player::BehaviorAttackInitialize() {

	attackTimer = 0;
	velocity_ = {0.0f, 0.0f, 0.0f};
}

void Player::BehaviorRootUpdate() {

	// 攻撃キーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	
		// 攻撃ビヘイビアをリクエスト
		behavierRequest_ = Behavior::kAttack;
	}

	Move();

	// 衝突判定を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;

	// マップ衝突チェック
	Collision(collisionMapInfo);

	CollisionUpdate(collisionMapInfo);
	CollisionWall(collisionMapInfo);
	SwitchLanding(collisionMapInfo);
	CollisionMove(collisionMapInfo);

	WorldTrnasformUpdate(worldTransform_);
}

void Player::BehaviorAttackUpdate() {

	// 攻撃動作用の速度
	Vector3 velocity{};

	attackTimer += 1.0f / 60.0f;

	float t;
	float easedT;

	switch (attackPhase_) {
	case Player::AttackPhase::accumulate:
	default:
		t = static_cast<float>(attackTimer) / kAccumulate;

		// イージング補間値を取得
		easedT = easeOutCubic(t);

		// 開始角度から終了角度へイージング補間
		worldTransform_.scale_.z = std::lerp(1.0f, 0.3f, easedT);
		worldTransform_.scale_.y = std::lerp(1.0f, 1.6f, easedT);

		// 全身動作へ平行
		if (attackTimer >= kAccumulate) {

			attackPhase_ = AttackPhase::rush;
			attackTimer = 0; // カウンターをリセット
		}

		break;
	case Player::AttackPhase::rush:

		if (lrDirection_ == LRDirection::kRight) {
			velocity.x = +attackVelocity;
		} else if (lrDirection_ == LRDirection::kLeft) {
			velocity.x = -attackVelocity;
		}

		t = static_cast<float>(attackTimer) / kRush;

		// イージング補間値を取得
		easedT = easeOutCubic(t);

		// 開始角度から終了角度へイージング補間
		worldTransform_.scale_.z = std::lerp(0.3f, 1.3f, easedT);
		worldTransform_.scale_.y = std::lerp(1.6f, 0.7f, easedT);

		// 全身動作へ平行
		if (attackTimer >= kRush) {

			attackPhase_ = AttackPhase::afterglow;
			attackTimer = 0;
		}

		break;
	case Player::AttackPhase::afterglow:

		t = static_cast<float>(attackTimer) / kAfterGraw;

		// イージング補間値を取得
		easedT = easeOutCubic(t);

		// 開始角度から終了角度へイージング補間
		worldTransform_.scale_.z = std::lerp(1.3f, 1.0f, easedT);
		worldTransform_.scale_.y = std::lerp(0.7f, 1.0f, easedT);

		if (attackTimer >= kAfterGraw) {
			attackPhase_ = AttackPhase::accumulate;
			behavierRequest_ = Behavior::kRoot;
		}

		break;
	}

	// 衝突判定を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity;

	// マップ衝突チェック
	Collision(collisionMapInfo);

	CollisionUpdate(collisionMapInfo);
	CollisionWall(collisionMapInfo);
	SwitchLanding(collisionMapInfo);
	CollisionMove(collisionMapInfo);

	// トランスフォームの値をコピー
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	WorldTrnasformUpdate(worldTransformAttack_);
	WorldTrnasformUpdate(worldTransform_);

}

void Player::Update() {

	if (behavierRequest_ != Behavior::kUnkown) {
	
		// ふるまいを変更する
		behavior_ = behavierRequest_;
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootinitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		// ふるまいクエストをリセット
		behavierRequest_ = Behavior::kUnkown;
	}

	// 更新
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}
}

void Player::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
	if (attackPhase_ == AttackPhase::rush || attackPhase_ == AttackPhase::afterglow) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

}

Vector3 Player::GetWorldPosition(){ 
	
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) { 
	(void)enemy;

	// 攻撃中は無敵
	if (IsAttack()) {
		return;
	}

	// デスフラグを立てる
	isDead = true;
}

#include "Player.h"
#include "ImGui.h"
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

}

void Player::Update() {

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
	// 着地フラグ
	bool landing = false;

	// 地面とのあたり判定
	// 加工中？
	if (velocity_.y < 0) {
	
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 2.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
	
		// 着地
		if (landing) {
		// めり込み排斥
			worldTransform_.translation_.y = 2.0f;
			// 摩擦で横方向が減速する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
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

	// 移動
	worldTransform_.translation_ += velocity_;

	// アフィン変換行列を計算してメンバ変数に代入
	KamataEngine::Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransform_.scale_);
	KamataEngine::Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransform_.translation_);
	KamataEngine::Matrix4x4 rotateXMatrix = MakeRotateXMatrix(worldTransform_.rotation_.x);
	KamataEngine::Matrix4x4 rotateYMatrix = MakeRotateYMatrix(worldTransform_.rotation_.y);
	KamataEngine::Matrix4x4 rotateZMatrix = MakeRotateZMatrix(worldTransform_.rotation_.z);
	KamataEngine::Matrix4x4 affineMatrix = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

	worldTransform_.matWorld_ = affineMatrix;

	// 定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}

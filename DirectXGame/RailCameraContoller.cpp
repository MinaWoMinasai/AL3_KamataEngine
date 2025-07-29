#include "RailCameraContoller.h"
using namespace KamataEngine;
using namespace MathUtility;
#include "Player.h"

void RailCameraController::Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation, KamataEngine::Camera* camera, Player* player) {

	// ワールドトランスフォームの初期設定
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	// カメラの初期化
	camera_ = camera;
	player_ = player;
}

void RailCameraController::Update() {

	t += 0.001f;
	// リセット
	if (t > 1.0f) {
		t = 0.0f;
	}

	// カメラ位置と注視点
	Vector3 eye = CatmullRomPosition(controlPoints_, t);
	Vector3 target = CatmullRomPosition(controlPoints_, std::min(t + 0.01f, 1.0f));
	
	Vector3 a = target - eye;
	Vector3 dir = Normalize(a);

	// カメラの回転を求める
	Vector3 rotation = VectorToRotation(dir);

	// カメラのワールドトランスフォーム更新
	Vector3 playerPosition = player_->GetWorldTransform().translation_;
	worldTransform_.translation_ = eye + playerPosition;
	worldTransform_.rotation_ = rotation;
	WorldTransformUpdate(worldTransform_);

	// ビュー行列を生成
	camera_->matView = Inverse(worldTransform_.matWorld_);
	
}

KamataEngine::Vector3 RailCameraController::GetWorldPosition() const {
	// ワールド座標を入れる
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

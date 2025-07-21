#include "RailCameraContoller.h"
using namespace KamataEngine;
using namespace MathUtility;

void RailCameraController::Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation, KamataEngine::Camera* camera) {

	// ワールドトランスフォームの初期設定
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	// カメラの初期化
	camera_ = camera;
}

void RailCameraController::Update() {

	// カメラの座標をImGuiで調整
	ImGui::Begin("Camera");
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x);
	ImGui::End();

	// 移動させる
	const Vector3 kMoveSpeed = {0.0f, 0.0f, 0.1f};
	//const Vector3 kRotateSpeed = {0.0f, 0.0001f, 0.0f};

	worldTransform_.translation_ += kMoveSpeed;
	//worldTransform_.rotation_ += kRotateSpeed;

	// ワールドトランスフォームの更新
	WorldTransformUpdate(worldTransform_);
	// カメラオブジェクトのワールド座標からビュー行列を計算する
	camera_->matView = Inverse(worldTransform_.matWorld_);

}
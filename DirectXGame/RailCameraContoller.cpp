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

	ImGui::Begin("Camera");
	ImGui::DragFloat("t", &t, 0.001f, 0.0f, 1.0f); // スプライン上の位置
	ImGui::End();

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
	worldTransform_.translation_ = eye;
	worldTransform_.rotation_ = rotation;
	WorldTransformUpdate(worldTransform_);

	// ビュー行列を生成
	camera_->matView = Inverse(worldTransform_.matWorld_);
	
}
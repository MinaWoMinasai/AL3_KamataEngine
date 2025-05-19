#include "CameraContoroller.h"
using namespace KamataEngine;
using namespace MathUtility;
#include "Player.h"

void CameraContoroller::Initialize(Camera* camera) {

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

}

void CameraContoroller::Reset() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;

}

void CameraContoroller::Update() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTrnasform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの目標座標を計算
	targetPos_ = targetWorldTrnasform.translation_ + targetOffset_;

	// 座標補間によりゆったり追従
	camera_->translation_ = Lerp(camera_->translation_, targetPos_, kInterpolationRate);

	// 移動範囲制限
	//camera_->translation_.x = std::clamp(camera_->translation_.x, movebleArea_.left, movebleArea_.right);
	//camera_->translation_.y = std::clamp(camera_->translation_.y, movebleArea_.bottom, movebleArea_.top);

	camera_->translation_.x = std::max(camera_->translation_.x, movebleArea_.left + kMovebleArea.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movebleArea_.right + kMovebleArea.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movebleArea_.bottom + kMovebleArea.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movebleArea_.top + kMovebleArea.top);

	// 行列を更新する
	camera_->UpdateMatrix();
}

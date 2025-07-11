#include "Enemy.h"
using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	switch (phase_) {
	case Enemy::Phase::Approach:
	default:

		ApproachUpdate();
		break;
	case Enemy::Phase::Leave:

		LeaveUpdate();
		break;
	}

	WorldTransformUpdate(worldTransform_);
}

void Enemy::ApproachUpdate() {
	
	// キャラクターの移動速さ
	const Vector3 approachMove = {0.0f, 0.0f, -0.1f};

	// 移動
	worldTransform_.translation_ += approachMove;
	// 既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {

	// キャラクターの移動速さ
	const Vector3 leaveMove = {0.1f, 0.1f, -0.1f};
	// 移動
	worldTransform_.translation_ += leaveMove;
}

void Enemy::Draw(KamataEngine::Camera& viewProjection) { model_->Draw(worldTransform_, viewProjection, textureHandle_); }

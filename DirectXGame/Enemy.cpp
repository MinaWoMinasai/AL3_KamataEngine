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

	// キャラクターの移動速さ
	const Vector3 move = {0.0f, 0.0f, -0.1f};

	worldTransform_.translation_ += move;

	WorldTransformUpdate(worldTransform_);

}

void Enemy::Draw(KamataEngine::Camera& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

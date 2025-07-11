#include "Enemy.h"
#include "ApproachState.h"
using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	
	SetState(std::make_unique<ApproachState>());
}

void Enemy::Update() {
	
	if (state_) {
		state_->Update(*this);
	}
	
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw(KamataEngine::Camera& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_); 
}

void Enemy::SetState(std::unique_ptr<BaseEnemyState> newState) {
	state_ = std::move(newState); 
}

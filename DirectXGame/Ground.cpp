#include "Ground.h"

void Ground::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -50.0f;
	WorldTransformUpdate(worldTransform_);
	camera_ = camera;
}

void Ground::Update() {}

void Ground::Draw() { model_->Draw(worldTransform_, *camera_); }

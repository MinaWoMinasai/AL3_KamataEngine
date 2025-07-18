#include "Skydome.h"

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	camera_ = camera;
}

void Skydome::Update() {}

void Skydome::Draw() { 
	model_->Draw(worldTransform_, *camera_);
}

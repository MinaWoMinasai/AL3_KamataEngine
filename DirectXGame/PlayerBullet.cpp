#include "PlayerBullet.h"
using namespace KamataEngine;
using namespace MathUtility;

void PlayerBullet::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity) {

	// Nullポインタチェック
	assert(model);
	model_ = model;

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;
}

void PlayerBullet::Update() {

	// 座標を移動させる
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	WorldTransformUpdate(worldTransform_);

}

void PlayerBullet::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);

}

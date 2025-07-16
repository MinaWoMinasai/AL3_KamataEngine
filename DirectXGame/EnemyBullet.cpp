#include "EnemyBullet.h"
using namespace KamataEngine;
using namespace MathUtility;

void EnemyBullet::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity) {

	// Nullポインタチェック
	assert(model);
	model_ = model;

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemyBullet.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;

	// z方向に伸びた形状
	worldTransform_.scale_ = {0.5f, 0.5f, 3.0f};

	// Y軸回り角度(Θy)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// X軸回り角度(0x)
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z));
}

void EnemyBullet::Update() {

	// 座標を移動させる
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	WorldTransformUpdate(worldTransform_);
}

void EnemyBullet::Draw(const Camera& camera) { 
	model_->Draw(worldTransform_, camera, textureHandle_);
}
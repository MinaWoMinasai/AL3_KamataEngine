#include "EnemyBullet.h"
#include "Player.h"
using namespace KamataEngine;
using namespace MathUtility;

void EnemyBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

void EnemyBullet::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity) {

	// Nullポインタチェック
	assert(model);
	model_ = model;

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemyBullet.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;

	// Y軸回り角度(Θy)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// X軸回り角度(0x)
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z));

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kCollisionAttributeEnemy | kCollisionAttributeEnemy);
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
	model_->Draw(worldTransform_, camera, textureHandle_); }

KamataEngine::Vector3 EnemyBullet::GetWorldPosition() const { 
	// ワールド座標を入れる
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

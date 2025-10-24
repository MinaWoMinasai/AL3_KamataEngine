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

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kCollisionAttributePlayer | kCollisionAttributePlayer);
	//worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
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

void PlayerBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

Vector3 PlayerBullet::GetWorldPosition() const {
	
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB PlayerBullet::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

AABB PlayerBullet::ComputeAABBAt(const Vector3& pos){
	AABB aabb;
	Vector3 halfSize = {kWidth / 2.0f, kHeight / 2.0f, kWidth / 2.0f};
	aabb.min = pos - halfSize;
	aabb.max = pos + halfSize;
	return aabb;
}
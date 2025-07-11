#include "Enemy.h"
#include "ApproachState.h"
using namespace KamataEngine;
using namespace MathUtility;

Enemy::~Enemy() {

	// bullet_の開放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Fire() {

	// 弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, -kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);

}

void Enemy::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	
	SetState(std::make_unique<ApproachState>());

	if (state_) {
		state_->Initialize(*this);
	}
}

void Enemy::Update() {
	
	if (state_) {
		state_->Update(*this);
	}
	
	for (EnemyBullet* bullet : bullets_) {
	
		bullet->Update();
	}

	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw(KamataEngine::Camera& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_); 

	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Enemy::SetState(std::unique_ptr<BaseEnemyState> newState) {
	state_ = std::move(newState); 
}

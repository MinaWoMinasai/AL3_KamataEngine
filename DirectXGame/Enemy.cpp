#include "Enemy.h"
#include "ApproachState.h"
using namespace KamataEngine;
using namespace MathUtility;

Enemy::~Enemy() {

	// bullet_の開放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	// TimeCallの解放
	for (TimeCall* timeCall : timeCalls_) {
		delete timeCall;
	}
}

void Enemy::Fire() {

	WorldTransformUpdate(worldTransform_);

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

void Enemy::FireAndReset() {
	
	// 弾を発射する
	Fire();

	// 発射タイマーをリセット
	timeCalls_.push_back(
		new TimeCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));

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
	
	for (TimeCall* timeCall : timeCalls_) {
		timeCall->Update();
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 終了したイベントを削除
	timeCalls_.remove_if([](TimeCall* timeCall) {
		if (timeCall->IsFinished()) {
			delete timeCall;
			return true;
		}
		return false;
	});

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

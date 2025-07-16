#include "Enemy.h"
#include "ApproachState.h"
#include "Player.h"
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

	assert(player_);

	// 弾の速度
	const float kBulletSpeed = 0.5f;

	// 自キャラの位置を取得
	Vector3 playerPos = player_->GetWorldPosition();
	// 敵キャラのワールド座標を取得
	Vector3 enemyPos = GetWorldPosition();
	// 敵キャラから自キャラへのベクトルを求める
	Vector3 direction = playerPos - enemyPos;
	// ベクトルの正規化
	direction = Normalize(direction);
	// ベクトルの長さを速さに合わせる
	direction *= kBulletSpeed;

	// 自機と同じ位置なら発射しない
	if (direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f) {
		return;
	}

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	// 自キャラを設定
	newBullet->SetPlayer(player_);
	newBullet->Initialize(model_, worldTransform_.translation_, direction);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Enemy::FireAndReset() {

	// 弾を発射する
	Fire();

	// 発射タイマーをリセット
	timeCalls_.push_back(new TimeCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));
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

void Enemy::SetState(std::unique_ptr<BaseEnemyState> newState) { state_ = std::move(newState); }

KamataEngine::Vector3 Enemy::GetWorldPosition() const {
	// ワールド座標を入れる
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

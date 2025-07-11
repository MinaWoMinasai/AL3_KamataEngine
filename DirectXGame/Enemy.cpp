#include "Enemy.h"
#include "cmath"
#include "Player.h"
#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 2.0f;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;

}

void Enemy::Update() {

	if (behavierRequest_ != Behavior::kUnkown) {

		// ふるまいを変更する
		behavior_ = behavierRequest_;
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kWalk:
		default:
			//BehaviorWalkinitialize();
			break;
		case Behavior::kDeath:
			//BehaviorAttackInitialize();
			break;
		}
		// ふるまいクエストをリセット
		behavierRequest_ = Behavior::kUnkown;
	}

	// 更新
	switch (behavior_) {
	case Behavior::kWalk:
	default:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}
}

void Enemy::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}

void Enemy::BehaviorWalkUpdate() {

	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(std::numbers::pi_v<float> * 2.0f * (walkTimer_ / kWalkMotionTime));
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.z = std::sin(radian);

	WorldTrnasformUpdate(worldTransform_);
}

void Enemy::BehaviorDeathUpdate() {

	deathTimer += 1.0f / 60.0f;

	float t;
	float easedT;

	t = static_cast<float>(deathTimer) / kDeathTimer;
	
	// イージング補間値を取得
	easedT = easeInElastic(t);

	// 開始角度から終了角度へイージング補間
	worldTransform_.scale_ = Lerp({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, easedT);
	

	if (deathTimer >= kDeathTimer) {
		isDead = true;
		behavior_ = Behavior::kUnkown;
	}

	WorldTrnasformUpdate(worldTransform_);
}

Vector3 Enemy::GetWorldPosition() {
	
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Enemy::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player, GameScene* gameScene) { 

	if (behavior_ == Behavior::kDeath) {
		// 敵がやられているなら何もしない
		return;
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	if (player->IsAttack()) {
		// 敵のふるまいをデス演出に変更
		behavierRequest_ = Behavior::kDeath;
		// コリジョン無効フラグを立てる
		isCollisionDisabled_ = true;

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos = (worldTransform_.translation_ + player->GetWorldTransform().translation_) / 2.0f;
		gameScene->CreateHitEffect(effectPos);
	}

}

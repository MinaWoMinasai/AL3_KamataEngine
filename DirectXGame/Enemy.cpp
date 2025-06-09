#include "Enemy.h"
#include "cmath"
using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, const KamataEngine::Vector3 playerpositon) {

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
	playerPosition_ = playerpositon;
}

void Enemy::Update(const KamataEngine::Vector3 playerpositon) {

	// ホーミング
	// プレイヤーの位置を格納
	
	playerPosition_ = playerpositon;

	Vector3 targetDir = Normalize(playerPosition_ - worldTransform_.translation_);

	velocity_ = Lerp(velocity_, targetDir * 0.1f, /*std::clamp(walkTimer_, 0.0f, 1.0f)*/0.01f);

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

void Enemy::OnCollision(const Player* player) { (void)player; }

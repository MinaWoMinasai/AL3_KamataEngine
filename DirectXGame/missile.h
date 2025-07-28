#pragma once
#include "Calculation.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "Collider.h"
#include "CollisionConfig.h"

class Enemy;

class Missile : public Collider {

public:
	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

	KamataEngine::Vector3 GetWorldPosition() const override;
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

private:
	KamataEngine::Model* model_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_;

	// 速さ
	static inline const float kBulletSpeed = 0.5f;

	// 寿命
	static const int32_t kLifeTime = 60 * 2;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	// 敵キャラ
	Enemy* enemy_ = nullptr;
};
#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "Calculation.h"

class PlayerBullet {

public:

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	KamataEngine::Vector3 GetMove() { return velocity_; }

	// セッター
	void SetWorldPosition(const KamataEngine::Vector3& pos) {
		worldTransform_.translation_ = pos;
		WorldTransformUpdate(worldTransform_);
	}

	AABB GetAABB();

	void SetVelocity(const KamataEngine::Vector3& v) { velocity_ = v; }
	AABB ComputeAABBAt(const KamataEngine::Vector3& pos);

private:

	KamataEngine::Model* model_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60000 * 2;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;
};

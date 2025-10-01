#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>

class PlayerBullet {

public:

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

private:

	KamataEngine::Model* model_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 2;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

};

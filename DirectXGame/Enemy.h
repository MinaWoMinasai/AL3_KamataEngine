#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "BaseEnemyState.h"
#include "EnemyBullet.h"

class Enemy {

public:
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	// 状態変更
	void SetState(std::unique_ptr<BaseEnemyState> newState);

	// 状態クラス用 Getter/Setter
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }
	int32_t& GetFireIntervalTimer() { return fireIntervalTimer; } 
	
	//----------------------
	// 定数
	
	// 発射間隔
	static inline const int32_t kFireInterval = 60;

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// State Pattern
	std::unique_ptr<BaseEnemyState> state_;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 発射タイマー
	int32_t fireIntervalTimer = 0;

};

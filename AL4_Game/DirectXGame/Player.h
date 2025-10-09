#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "PlayerBullet.h"
#include <list>
#include "Calculation.h"

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 旋回
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	void RotateToMouse(const KamataEngine::Camera& viewProjection);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const KamataEngine::Camera& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// セッター
	void SetWorldPosition(const KamataEngine::Vector3& pos) {
		worldTransform_.translation_ = pos;
		WorldTransformUpdate(worldTransform_);
	}

	// AABBを取得
	AABB GetAABB();
	KamataEngine::Vector2 GetMoveInput();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformMouse_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

	// 弾
	std::list<PlayerBullet*> bullets_;
	KamataEngine::Vector3 dir;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;
	
	// キャラクターの移動速さ
	float characterSpeed = 0.2f;
};


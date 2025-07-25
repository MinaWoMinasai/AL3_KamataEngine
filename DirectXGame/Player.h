#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "PlayerBullet.h"
#include <list>
#include "Collider.h"
#include "CollisionConfig.h"
#include "Calculation.h"

class LockOn;
//class Enemy;

/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider{

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

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Model* bulletModel, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const KamataEngine::Camera& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	KamataEngine::Vector3 GetWorldPosition() const override;
	KamataEngine::Vector3 GetWorldPosition3DReticle() const;
	// 弾の取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	KamataEngine::Vector2 GetReticlePos2D() const { return reticlePos2D; }

	// 半径
	static inline const float kRadius = 1.0f;

	void SetParent(const KamataEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }
	void SetParent3DReticle(const KamataEngine::WorldTransform* parent) { worldTransform3DReticle_.parent_ = parent; }

	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* bulletModel_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

	// 弾
	std::list<PlayerBullet*> bullets_;

	// 3dレティクル用ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform3DReticle_;

	// 2Dレティクル用スプライト
	KamataEngine::Sprite* sprite2DReticle_ = nullptr;
	
	// ビュープロジェクション
	KamataEngine::Camera* viewProjection_;

	KamataEngine::Vector3 rayDirection_;

	// 2dレティクルでの位置
	KamataEngine::Vector2 reticlePos2D;

	LockOn* lockOn_ = nullptr;

};


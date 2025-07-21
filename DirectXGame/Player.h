#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "PlayerBullet.h"
#include <list>
#include "Collider.h"
#include "CollisionConfig.h"

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
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Model* bulletModel, const KamataEngine::Vector3& position );

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	KamataEngine::Vector3 GetWorldPosition() const override;

	// 弾の取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	// 半径
	static inline const float kRadius = 1.0f;

	void SetParent(const KamataEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }

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

};


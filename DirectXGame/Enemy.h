#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include <numbers>
#include <vector>
#include "Easing.h"
#include <algorithm>
#include "MapChipField.h"
#include "WorldTransformClass.h"
#include "HitBox.h"

class Player;
class GameScene;

class Enemy {

public:

	enum class Behavior {
		kWalk,   // 移動
		kDeath, // 死亡
		kUnkown, // 待機中
	};

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 移動更新
	/// </summary>
	void BehaviorWalkUpdate();

	/// <summary>
	/// 死亡更新
	/// </summary>
	void BehaviorDeathUpdate();

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();
	
	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player, GameScene* gameScene);

	// デスフラグのgetter
	bool IsDead() const { return isDead; }

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};
	// 経過時間
	float walkTimer_ = 0.0f;

	// デスフラグ
	bool isDead = false;

	// 判定無効フラグ
	bool isCollisionDisabled_ = false;

	// 死亡タイマー
	float deathTimer = 0.0f;

	static inline const float kDeathTimer = 0.6f;

	// ふるまい
	Behavior behavior_ = Behavior::kWalk;

	// 次のふるまい
	Behavior behavierRequest_ = Behavior::kUnkown;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	// 最初の角度
	static inline const float kWalkMotionAngleStart = std::numbers::pi_v<float> * 2.0f;
	// 最後の角度
	static inline const float kWalkMotionAngleEnd = std::numbers::pi_v<float>; 
	// アニメーションの周期になる時間[秒]
	static inline const float kWalkMotionTime = 2.0f;

};

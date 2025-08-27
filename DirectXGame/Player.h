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

// 前方宣言
class Enemy;
class MapChipField;

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:

	~Player();

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner,
	};

	struct CollisionMapInfo {
		// 天井衝突フラグ
		bool ceiling = false;
		// 着地フラグ
		bool landing = false;
		// 壁接触フラグ
		bool hitWall = false;
		// 移動量
		KamataEngine::Vector3 velocity = {};
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	KamataEngine::Vector3 CornerPositon(const KamataEngine::Vector3& center, Corner corner);

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// マップ衝突判定上
	/// </summary>
	/// <param name="info"></param>
	void CollisionTop(CollisionMapInfo& info);

	void CollisionBottom(CollisionMapInfo& info);

	void CollisionRight(CollisionMapInfo& info);
	void CollisionLeft(CollisionMapInfo& info);
	
	void SwitchLanding(const CollisionMapInfo& info);

	void CollisionWall(const CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定
	/// </summary>
	/// <param name="info"></param>
	void Collision(CollisionMapInfo& info);

	void CollisionMove(const CollisionMapInfo& info);

	void CollisionUpdate(const CollisionMapInfo& info);
	
	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void BehaviorRootinitialize();

	void BehaviorAttackInitialize();

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールドトランスフォームを取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 攻撃フラグのgetter
	bool IsAttack() const { return behavior_ == Behavior::kAttack; }

	// デスフラグのgetter
	bool IsDead() const { return isDead; }

	bool IsClear() const { return isClear; }

	// プレイヤーの突進タイマー
	uint32_t GetPlayerAttackTimer;
	// プレイヤーの突撃カウント
	uint32_t GetPlayerAttackCount;

private:

	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum class Behavior {
		kRoot,  // 通常攻撃
		kAttack, // 攻撃中
		kUnkown, // 待機中
	};

	enum class AttackPhase {
		accumulate, // 溜め
		rush, // 突進
		afterglow, // 余韻
		none,
	};

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformAttack_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	// 加速度
	static inline const float kAcceleration = 0.02f;
	// 摩擦
	static inline const float kAttenuation = 0.05f;
	// どこを向いているか
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 1.0f;
	// 最大速度
	static inline const float kLimitRunSpeed = 0.3f;
	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.01f;
	// 最大落下速度(下方向)
	static inline const float kLimitFailSpeed = 0.8f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.35f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;

	static inline const float kBlank = 0.05f;
	
	// 接地時の判定の調整
	static inline const float kLandingAdjust = 0.1f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;
	static inline const float kAttenuationWall = 0.1f;

	// デスフラグ
	bool isDead = false;
	
	// ダッシュタイマー
	float attackTimer = 0.0f;

	static inline const float kAttackTimer = 3.0f;

	// ふるまい
	Behavior behavior_ = Behavior::kRoot;

	// 次のふるまい
	Behavior behavierRequest_ = Behavior::kUnkown;

	// 攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::accumulate;

	// 溜め動作時間
	static inline const float kAccumulate = (1.0f / 30.0f);
	// 突進動作時間
	static inline const float kRush = (1.0f / 5.0f);
	
	
	// 溜めマックスと最小と増加速度
	static inline const float kRushMax = (1.0f / 2.0f);
	static inline const float kRushMin = (1.0f / 10.0f);
	static inline const float kRushSpeed = (1.0f / 35.0f);
	float rush_ = kRushMin;

	// 余韻動作時間
	static inline const float kAfterGraw = (1.0f / 15.0f);
	// 攻撃時の速度
	static inline const float kAttackVelocity = 0.55f;
	float attackVelocity = kAttackVelocity;


	// 突撃回数
	uint32_t attackCount = 2;

	// 突撃最大回数
	static inline const uint32_t kAttackMax = 2;

	// 突撃が復活するまで
	static inline const uint32_t kAttackResetTimer = 65;

	// 突撃復活タイマー
	uint32_t attackResetTimer = 0;

	// 溜めているか
	bool isTame = false;

	// クリアしたか
	bool isClear = false;

	KamataEngine::Sprite* arrow_ = nullptr;
	KamataEngine::Sprite* arrow2_ = nullptr;
	KamataEngine::Sprite* gide_ = nullptr;
	KamataEngine::Vector2 spritePos;
	KamataEngine::Vector2 spriteSize;
	float spriteRectX;

	uint32_t textureHandleG_ = 0u;
};


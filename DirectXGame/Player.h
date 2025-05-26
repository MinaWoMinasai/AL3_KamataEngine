#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include <numbers>
#include <vector>
#include "Easing.h"
#include <algorithm>
#include "MapChipField.h"

class MapChipField;

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:

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
		bool wall = false;
		// 移動量
		KamataEngine::Vector3 move = {};
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="camera">カメラ</param>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

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

	/// <summary>
	/// マップ衝突判定
	/// </summary>
	/// <param name="info"></param>
	void Collision(CollisionMapInfo& info);

	void CollisionMove(const CollisionMapInfo& info);

	void CollisionUpdate(const CollisionMapInfo& info);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールドトランスフォームを取得
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

private:

	enum class LRDirection {
		kRight,
		kLeft,
	};

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	// 加速度
	static inline const float kAcceleration = 0.01f;
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
	static inline const float kLimitRunSpeed = 0.5f;
	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.003f;
	// 最大落下速度(下方向)
	static inline const float kLimitFailSpeed = 3.0f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.2f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;

	static inline const float kBlank = 0.2f;

};


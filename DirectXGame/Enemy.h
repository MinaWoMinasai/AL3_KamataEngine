#pragma once
#include "BaseEnemyState.h"
#include "EnemyBullet.h"
#include "KamataEngine.h"
#include "TimeCall.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>
#include "Collider.h"
#include "CollisionConfig.h"

class Player;
class GameScene;

class Enemy : public Collider{

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
	/// 弾を発射し、タイマーをリセットするコールバック関数
	/// </summary>
	void FireAndReset();

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Vector3& position, KamataEngine::Model* modelBullet);

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

	KamataEngine::Vector3 GetWorldPosition() const override;
	// 次元発動のリストのゲッター
	std::list<TimeCall*>& GetTimeCalls() { return timeCalls_; }

	// 自キャラのセッター
	void SetPlayer(Player* player) {player_ = player;}

	// ゲームシーンのセッター
	void SetGameScene(GameScene* gameScene) {gameScene_ = gameScene;}

	// デスフラグのゲッター
	bool IsDead() const { return isDead_; }

	//----------------------
	// 定数

	// 発射間隔
	static inline const int32_t kFireInterval = 60;

	// 半径
	static inline const float kRadius = 1.0f;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* bulletModel_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// State Pattern
	std::unique_ptr<BaseEnemyState> state_;

	// 発射タイマー
	int32_t fireIntervalTimer = 0;

	// 時限発動のリスト
	std::list<TimeCall*> timeCalls_;

	// 自キャラ
	Player* player_ = nullptr;

	// 最初の弾までの時間
	uint32_t time_ = 60;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;
	// デスフラグ
	bool isDead_ = false;
};

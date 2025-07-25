#pragma once
#include "KamataEngine.h"
#include <Windows.h>
#include "Player.h"
#include "Enemy.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "Skydome.h"
#include "Ground.h"
#include "RailCameraContoller.h"
#include <sstream>
#include "LockOn.h"

// ゲームシーン
class GameScene {

public:

	// コピー禁止
	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	GameScene& operator=(GameScene&&) = delete;

	/// <summary>
	/// 敵の弾を追加する
	/// </summary>
	/// <param name="enemyBullet"></param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	/// <summary>
	/// 敵の発生
	/// </summary>
	void EnemyPop(KamataEngine::Vector3 position);

private:

	// ビュープロジェクション
	KamataEngine::Camera viewProjection_;

	// 自キャラ
	Player* player_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 複数の敵のリスト
	std::list<Enemy*> enemies_;

	// テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;

	// モデル
	KamataEngine::Model *playerModel_;
	KamataEngine::Model* enemyModel_;
	KamataEngine::Model* skydomeModel_ = nullptr;
	KamataEngine::Model* playerBulletModel_ = nullptr;
	KamataEngine::Model* enemyBulletModel_ = nullptr;
	KamataEngine::Model* groundModel_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

	// 衝突マネージャ
	CollisionManager* collisionManager_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	// 地面
	Ground* ground_ = nullptr;

	// レールカメラ
	RailCameraController* railCameraController_ = nullptr;

	// 曲線描画テスト
	std::vector<KamataEngine::Vector3> controlPoints_;

	// 敵の弾
	std::list<EnemyBullet*> enemyBullets_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;

	// 待機中フラグ
	bool isWaiting_ = false;
	// 待機時間
	int32_t waitTime_ = 0;
	std::vector<std::string> enemyPopLines_;
	size_t currentLineIndex_ = 0;

	// ロックオン
	LockOn* lockOn_ = nullptr;
};

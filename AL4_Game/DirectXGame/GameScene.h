#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include "Player.h"
#include "Enemy.h"
#include "MapChip.h"
#include "Calculation.h"
#include <sstream>
#include "Collider.h"
#include "CollisionManager.h"

// ゲームシーン
class GameScene {

public:

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
	/// マップチップの生成
	/// </summary>
	void GeneratteBlocks();

	/// <summary>
	/// プレイヤーとブロックの当たり判定
	/// </summary>
	void CheckCollisionPlayerAndBlocks(char axis);

	void CheckCollisionBulletsAndBlocks();

	/// <summary>
	/// 敵の弾を追加する
	/// </summary>
	/// <param name="enemyBullet"></param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

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
	void EnemyPop(KamataEngine::Vector3 position, const std::string& behaviorName);

private:

	struct Block {
		std::unique_ptr<KamataEngine::WorldTransform> worldTransform;
		AABB aabb;
	};

	// ビュープロジェクション
	KamataEngine::Camera viewProjection_;

	// 自キャラ
	Player* player_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 複数の敵のリスト
	std::list<Enemy*> enemies_;

	// 敵の弾
	std::list<EnemyBullet*> enemyBullets_;

	// テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;

	// モデル
	KamataEngine::Model* playerModel_;
	KamataEngine::Model* enemyModel_;
	std::unique_ptr<KamataEngine::Model> modelBlock_ = nullptr;
	std::unique_ptr<KamataEngine::Model> modelBullet_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

	// ブロック用のワールドトランスフォーム
	std::vector<std::vector<Block>> blocks_;
	// マップチップ
	std::unique_ptr<MapChip> mapChip_ = nullptr;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;

	// 待機中フラグ
	bool isWaiting_ = false;
	// 待機時間
	int32_t waitTime_ = 0;
	std::vector<std::string> enemyPopLines_;
	size_t currentLineIndex_ = 0;

	// 衝突マネージャ
	CollisionManager* collisionManager_ = nullptr;
	KamataEngine::Vector3 normal_;
};

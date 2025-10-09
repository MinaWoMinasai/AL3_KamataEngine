#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include "Player.h"
#include "Enemy.h"
#include "MapChip.h"
#include "Calculation.h"

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
	void CheckCollisionPlayerAndBlocks();

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

	// 敵キャラ
	Enemy* enemy_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;

	// モデル
	KamataEngine::Model* playerModel_;
	KamataEngine::Model* enemyModel_;
	std::unique_ptr<KamataEngine::Model> modelBlock_ = nullptr;

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
};

#pragma once
#include "KamataEngine.h"
#include <Windows.h>
#include "Player.h"
#include "Enemy.h"

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
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

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

private:

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
	KamataEngine::Model *playerModel_;
	KamataEngine::Model* enemyModel_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// キーボード入力
	KamataEngine::Input* input_ = nullptr;

};

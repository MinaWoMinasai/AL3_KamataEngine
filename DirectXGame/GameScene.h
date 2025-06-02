#pragma once
#include "KamataEngine.h"
#include <Windows.h>
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraContoroller.h"
#include "Enemy.h"
#include "HitBox.h"

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
	/// 
	/// </summary>
	void GeneratteBlocks();

	// そうあたり判定を行う
	void CheakAllCollisions();

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
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;

	// 敵の数
	const int32_t kEnemyCount = 3;

	Skydome* skydome_ = nullptr; 

	// ブロックの3Dモデル
	KamataEngine::Model* modelBlock_ = nullptr;

	// ブロック用のワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラコントローラー
	CameraContoroller* cameraContoroller_ = nullptr;

};

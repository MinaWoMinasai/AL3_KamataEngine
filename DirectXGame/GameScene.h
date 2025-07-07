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
#include "DeathParticles.h"
#include "Fade.h"

// ゲームシーン
class GameScene {

// ゲームのフェーズ(型)
enum class Phase {
	kFadeIn,
	kPley, // ゲームプレイ
	kDeath, // デス演出
	kFadeOut,
};

// ゲームの現在フェーズ
Phase phase_;

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

	// シーンチェンジ
	void ChengePhase();

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

	// 終了フラグのゲッター
	bool IsFinished() const { return finished_; }

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* playerAttackModel_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelTitleText_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	DeathParticles* deathParticles_ = nullptr;
	
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

	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;

	// フェード時間
	static inline const float kDuration = 2.0f;

};

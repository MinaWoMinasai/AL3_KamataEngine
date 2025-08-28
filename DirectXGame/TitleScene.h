#pragma once
#include "KamataEngine.h"
#include <Windows.h>
#include "GameScene.h"
#include "WorldTransformClass.h"
#include <numbers>
#include "Fade.h"


class TitleScene {

public:

	enum class Phase { 
		kFadeIn,
		kMain,
		kFadeOut,
	};


	~TitleScene();

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
	// 終了フラグ
	bool finished_ = false;

	// モデル
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelText_ = nullptr;

	// プレイヤーと文字のワールドトランスフォーム
	KamataEngine::WorldTransform playerWorldTransform;
	KamataEngine::WorldTransform textWorldTransform;

	// プレイヤー
	Player *player_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	float time = 0.0f;

	// フェード
	Fade* fade_ = nullptr;

	// フェード時間
	static inline const float kDuration = 1.5f; 

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// 選択SE
	uint32_t sHSelect = 0;
};

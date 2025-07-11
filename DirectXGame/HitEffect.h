#pragma once
#include "WorldTransformClass.h"
#include <KamataEngine.h>
#include <iostream>
#include <numbers>
#include <random>

class HitEffect {

public:
	enum class Status {
		None,
		Expansion,
		FadeOut,
		Death,
	};

	// メルセンヌ・ツイスターエンジンの初期化
	HitEffect() : randomEngine(seedGenerator()) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool IsDead() const { return status_ == Status::Death; }

	// 乱数の生成
	float GetRamdom(const float& min, const float& max);

	static HitEffect* Create(const KamataEngine::Vector3& position);

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

private:
	// モデル
	static KamataEngine::Model* model_;
	// カメラ
	static KamataEngine::Camera* camera_;

	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	// 楕円のワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, 2> ellipseWorldTransforms_;

	// 乱数生成エンジン
	std::random_device seedGenerator;
	// メルセンヌ・ツイスターエンジン
	std::mt19937_64 randomEngine;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	static inline float kDuration = 0.2f;
	// 経過時間カウンター
	float counter_ = 0.0f;
	
};

#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include <numbers>
#include <vector>
#include "Easing.h"
#include <algorithm>
#include "MapChipField.h"
#include "WorldTransformClass.h"
#include "HitBox.h"
#include <array>

class DeathParticles {

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// 終了フラグのゲッター
	bool IsFinished() const { return isFinished_; }

private:

	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	// 存続時間(消滅までの時間)<秒>
	static inline const float kDuraction = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.2f;
	// 分割した一個分の角度
	static inline const float kAngleUnit = std::numbers::pi_v<float> * 2 / kNumParticles;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	// 色の情報
	KamataEngine::Vector4 color_;

};

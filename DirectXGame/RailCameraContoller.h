#pragma once
#define NOMINMAX
#include <KamataEngine.h>
#include "WorldTransformClass.h"
#include "Calculation.h"

class Player;

class RailCameraController {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation, KamataEngine::Camera* camera, Player* player);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// カメラのゲッター
    const KamataEngine::Camera* GetCamera() const { return camera_; }

	// ワールド座標を取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	KamataEngine::Vector3 GetWorldPosition() const;

	void SetControlPoints(const std::vector<KamataEngine::Vector3>& points) { controlPoints_ = points; }

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera* camera_;

	std::vector<KamataEngine::Vector3> controlPoints_;

	float t;

	Player* player_;
};

#pragma once
#include <KamataEngine.h>
#include "WorldTransformClass.h"

class RailCameraController {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation, KamataEngine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// カメラのゲッター
    const KamataEngine::Camera* GetCamera() const { return camera_; }

	// ワールド座標を取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera* camera_;

};

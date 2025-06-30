#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include <Windows.h>
#include <algorithm>

class PlayerBullet {

public:

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

private:

	KamataEngine::Model* model_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;


};

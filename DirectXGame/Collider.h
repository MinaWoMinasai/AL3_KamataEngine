#pragma once
#include <KamataEngine.h>

class Collider {

public:

	// 半径を取得
	float GetRadius() const { return radius_; }

	// 半径を設定
	void SetRadius(float radius) { radius_ = radius; }

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	virtual KamataEngine::Vector3 GetWorldPosition() const = 0;

	/// <summary>
	/// 衝突判定
	/// </summary>
	virtual void OnCollision() = 0;

private:

	// 衝突半径
	float radius_ = 1.0f;
};

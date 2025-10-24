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

	// 衝突属性(自分)を取得
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	// 衝突属性(自分)を設定
	void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }
	// 衝突マスク(相手)を取得
	uint32_t GetCollisionMask() const { return collisionMask_; }
	// 衝突マスク(相手)を設定
	void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }

private:
	// 衝突半径
	float radius_ = 0.8f;

	// 衝突属性
	uint32_t collisionAttribute_ = 0xffffffff;
	// 衝突マスク
	uint32_t collisionMask_ = 0xffffffff;
};

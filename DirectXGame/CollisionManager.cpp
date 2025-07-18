#include "CollisionManager.h"
using namespace KamataEngine;
using namespace MathUtility;
#include "Player.h"
#include "Enemy.h"

void CollisionManager::CheckAllCollisions(Player* player, Enemy* enemy) {

	// 衝突マネージャのリストをクリア
	colliders_.clear();

	// コライダーをリストに登録
	SetColliders(player, enemy);

	// リスト内のペアの総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {

		// イテレータAからコライダーAを取得
		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {

			// イテレータBからコライダーBを取得
			Collider* colliderB = *itrB;

			// ペアのあたり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	Vector3 posA = colliderA->GetWorldPosition();
	Vector3 posB = colliderB->GetWorldPosition();

	float distance = Length(posA - posB);
	// 球と球のあたり判定
	if (distance < colliderA->GetRadius() + colliderB->GetRadius()) {

		// 衝突フィルタリング
		if (
		    // ビットAND判定
		    (colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) || (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
			return;
		}

		// 自弾のデスフラグを立てる
		colliderA->OnCollision();
		// 敵弾のデスフラグを立てる
		colliderB->OnCollision();
	}
}

void CollisionManager::SetColliders(Player* player, Enemy* enemy) {

	// コライダーをリストに登録
	colliders_.push_back(player);
	colliders_.push_back(enemy);
	for (PlayerBullet* bullet : player->GetBullets()) {
		colliders_.push_back(bullet);
	}
	for (EnemyBullet* bullet : enemy->GetBullets()) {
		colliders_.push_back(bullet);
	}
}

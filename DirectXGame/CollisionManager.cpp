#include "CollisionManager.h"
using namespace KamataEngine;
using namespace MathUtility;
#include "Player.h"
#include "Enemy.h"
#include "EnemyBullet.h"

void CollisionManager::CheckAllCollisions(Player* player, const std::list<Enemy*>& enemies, const std::list<EnemyBullet*>& enemyBullets) {

	// 衝突マネージャのリストをクリア
	colliders_.clear();

	// コライダーをリストに登録
	SetColliders(player, enemies, enemyBullets);

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

void CollisionManager::SetColliders(Player* player, const std::list<Enemy*>& enemies, const std::list<EnemyBullet*>& enemyBullets) {

	// プレイヤーを登録
	colliders_.push_back(player);

	// プレイヤーの弾を登録
	for (PlayerBullet* bullet : player->GetBullets()) {
		colliders_.push_back(bullet);
	}

	// プレイヤーのミサイルを登録
	for (Missile* missile : player->GetMissiles()) {
		colliders_.push_back(missile);
	}

	// 複数の敵を登録
	for (Enemy* enemy : enemies) {
		colliders_.push_back(enemy);
	}

	// 敵の弾を登録（敵が直接持っていないパターン）
	for (EnemyBullet* bullet : enemyBullets) {
		colliders_.push_back(bullet);
	}
}
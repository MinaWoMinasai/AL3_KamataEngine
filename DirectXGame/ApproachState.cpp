#include "ApproachState.h"
#include "LeaveState.h"
using namespace KamataEngine;
using namespace MathUtility;

void ApproachState::Initialize(Enemy& enemy) {
	// 発射タイマーを初期化
	enemy.GetFireIntervalTimer() = enemy.kFireInterval;
}

void ApproachState::Update(Enemy& enemy) {
	// 移動処理
	const Vector3 approachMove = {0.0f, 0.0f, -0.1f};
	enemy.GetWorldTransform().translation_ += approachMove;

	// 発射タイマーカウントダウン
	enemy.GetFireIntervalTimer()--;

	// 指定時間に達した
	if (enemy.GetFireIntervalTimer() <= 0) {
		// 弾を発射
		enemy.Fire();
		// 発射タイマーを初期化
		enemy.GetFireIntervalTimer() = enemy.kFireInterval;
	}

	// 離脱条件をチェック
	if (enemy.GetWorldTransform().translation_.z < 0.0f) {
		enemy.SetState(std::make_unique<LeaveState>());
	}
}
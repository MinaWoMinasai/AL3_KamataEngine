#include "ApproachState.h"
#include "LeaveState.h"
using namespace KamataEngine;
using namespace MathUtility;

void ApproachState::Update(Enemy& enemy) {
	// 移動処理
	const Vector3 approachMove = {0.0f, 0.0f, -0.1f};
	enemy.GetWorldTransform().translation_ += approachMove;

	// 離脱条件をチェック
	if (enemy.GetWorldTransform().translation_.z < 0.0f) {
		enemy.SetState(std::make_unique<LeaveState>());
	}
}
#include "FallState.h"
#include "LeaveState.h"
using namespace KamataEngine;
using namespace MathUtility;

void FallState::Initialize(Enemy& enemy) {
	// 発射タイマーを初期化
	enemy.FireAndReset();
}

void FallState::Update(Enemy& enemy) {
	// 移動処理
	const Vector3 approachMove = {0.0f, 0.0f, -0.1f};
	enemy.GetWorldTransform().translation_ += approachMove;

	// 離脱条件をチェック
	if (enemy.GetWorldTransform().translation_.z < 0.0f) {
		enemy.SetState(std::make_unique<LeaveState>());
		// リストをクリアする
		for (TimeCall* timeCall : enemy.GetTimeCalls()) {
			delete timeCall;
			timeCall = nullptr;
		}
		enemy.GetTimeCalls().clear();
	}
}
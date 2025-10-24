#include "SniperState.h"

void SniperState::Initialize(Enemy& enemy) {
	// 発射タイマーを初期化
	enemy.FireAndReset();
}

void SniperState::Update(Enemy& enemy) { (void)enemy; }

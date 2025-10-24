#include "WaveState.h"
using namespace KamataEngine;
using namespace MathUtility;

void WaveState::Initialize(Enemy& enemy) {
	// 発射タイマーを初期化
	enemy.FireAndReset();
}

void WaveState::Update(Enemy& enemy) {
	// 上下に動く
	timer_ += 0.5f;

	const float waveAmplitude = 0.1f;  // 振幅
	const float waveFrequency = 0.05f; // 周波数

	float t = timer_;

	enemy.GetWorldTransform().translation_.y = enemy.GetWorldPosition().y + std::sin(t * waveFrequency) * waveAmplitude;
}

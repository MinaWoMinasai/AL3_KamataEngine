#pragma once
class Enemy;

class BaseEnemyState {
public:
	virtual ~BaseEnemyState() = default;

	// 状態ごとの更新処理
	virtual void Update(Enemy& enemy) = 0;
};

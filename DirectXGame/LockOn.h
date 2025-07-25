#pragma once
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"

class LockOn {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Player* player, std::list<Enemy*>& enemies, const KamataEngine::Camera& camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	Enemy* GetTarget() const { return target_; }

private:
	KamataEngine::Sprite* spriteLockOn_;

	// ロック対象
	Enemy* target_ = nullptr;
};

#pragma once
#include <KamataEngine.h>
#include "Enemy.h"
#include "Player.h"
#include <list>

class LockOn {
public:

	~LockOn();

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

	/// <summary>
	/// ロックオン解除
	/// </summary>
	void ClearTargets();

	const std::vector<Enemy*>& GetTargets() const { return targets_; }

private:
	
	std::vector<KamataEngine::Sprite*> sprites_; // ロックオン対象ごとのスプライト
	std::vector<Enemy*> targets_;  // ロックオン対象
	uint32_t textureHandle_;       // ロックオンマークのテクスチャ

	bool isLockOnFixed_ = false; // 一度ロックオンされたらtrueにして維持
};

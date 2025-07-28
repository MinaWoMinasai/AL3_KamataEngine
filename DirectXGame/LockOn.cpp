#include "LockOn.h"
using namespace KamataEngine;
using namespace MathUtility;

LockOn::~LockOn() { 
	for (Sprite* sprite : sprites_) {
		delete sprite;
	}
}

void LockOn::Initialize() {

	// レティクル用テクスチャ取得
	textureHandle_ = TextureManager::Load("2dReticle.png");
}

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const KamataEngine::Camera& camera) {

	// 古いターゲットの生存確認
	for (size_t i = 0; i < targets_.size();) {
		if (targets_[i] == nullptr || targets_[i]->IsDead()) {
			targets_.erase(targets_.begin() + i);
			sprites_.erase(sprites_.begin() + i);
		} else {
			++i;
		}
	}

	// ロックオン候補を毎フレーム確認（既に固定されてても追加できるようにする）
	for (Enemy* enemy : enemies) {

		if (!enemy || enemy->IsDead())
			continue;

		// 既にロックオンされていれば無視（重複防止）
		if (std::find(targets_.begin(), targets_.end(), enemy) != targets_.end())
			continue;

		Vector3 posWorld = enemy->GetWorldPosition();
		Vector3 posView = TransformCoord(posWorld, camera.matView);
		if (posView.z <= 0.0f)
			continue;

		Vector3 screenPos = Project(posWorld, 0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, camera.matView * camera.matProjection);
		float distance = Length2D(player->GetReticlePos2D(), Vector2(screenPos.x, screenPos.y));

		if (distance <= 100.0f) {
			// 新しくロックオン追加
			targets_.push_back(enemy);
			Sprite* sprite = Sprite::Create(textureHandle_, {screenPos.x, screenPos.y}, {1, 1, 1, 1}, {0.5f, 0.5f});
			sprites_.push_back(sprite);
		}
	}

	// スプライト位置を毎フレーム更新
	for (size_t i = 0; i < targets_.size(); ++i) {
		if (targets_[i]) {
			Vector3 screenPos = Project(targets_[i]->GetWorldPosition(), 0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, camera.matView * camera.matProjection);
			sprites_[i]->SetPosition(Vector2(screenPos.x, screenPos.y));
		}
	}

	isLockOnFixed_ = !targets_.empty();
	player->SetLockOn(this);
}

void LockOn::Draw() {

	for (size_t i = 0; i < targets_.size(); ++i) {
		if (i >= sprites_.size())
			continue;

		Enemy* enemy = targets_[i];
		if (enemy == nullptr || enemy->IsDead())
			continue;

		sprites_[i]->Draw();
	}
}

void LockOn::ClearTargets() {
	targets_.clear();
	isLockOnFixed_ = false;
}
